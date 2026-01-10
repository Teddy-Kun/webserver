use std::{
    collections::HashMap,
    fs,
    io::{BufRead, BufReader, Write},
    net::{TcpListener, TcpStream},
};

use crate::{
    error::ThinError,
    http::{HttpMethod, HttpResponse, HttpStatusCode},
};

mod error;
mod http;

fn main() {
    if let Err(e) = create_listener() {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}

fn create_listener() -> Result<(), ThinError> {
    let listener = TcpListener::bind("127.0.0.1:7878")?;

    for stream in listener.incoming() {
        let mut stream = match stream {
            Ok(s) => s,
            Err(e) => {
                eprintln!("Error reading stream: {}", e);
                continue;
            }
        };

        if let Err(e) = handle_connection(&mut stream) {
            eprintln!("Error handling connection: {}", e);
            let response = HttpResponse::new(HttpStatusCode::InternalServerError).into_string();
            println!("{response}");
            if let Err(e) = stream.write_all(response.as_bytes()) {
                eprintln!("Error handling connection: {}", e);
            }
        }
    }

    Ok(())
}

fn handle_connection(stream: &mut TcpStream) -> Result<(), ThinError> {
    let buf_reader = BufReader::new(&*stream);

    let mut lines = buf_reader.lines();

    let head = lines.next().ok_or(ThinError::str("missing http head"))??;

    let mut splitter = head.split_ascii_whitespace();

    let method = splitter
        .next()
        .ok_or(ThinError::str("missing http method"))?;
    let req_uri = splitter
        .next()
        .ok_or(ThinError::str("missing http request-uri"))?;
    let version = splitter
        .next()
        .ok_or(ThinError::str("missing http version"))?;

    let method = HttpMethod::try_from(method)?; // validate and convert str to our enum

    println!("{method} {req_uri} {version}");

    if version != "HTTP/1.1" {
        let response = HttpResponse::new(HttpStatusCode::HttpVersionNotSupported);
        stream.write_all(response.into_string().as_bytes())?;
        return Ok(());
    }

    if method != HttpMethod::Get {
        return Err(ThinError::new(
            format!("unsupported http method: {method}").into(),
        ));
    }

    let headers: HashMap<Box<str>, Box<str>> = lines
        .map_while(Result::ok)
        .take_while(|line| !line.is_empty())
        .map(|line| {
            let (key, value) = line
                .split_once(':')
                .ok_or(ThinError::str("missing http header delimiter"))?;

            let key = key.trim();
            let value = value.trim();

            if key.is_empty() {
                return Err(ThinError::str("http header key is empty"));
            }

            if value.is_empty() {
                return Err(ThinError::str("http header value is empty"));
            }

            Ok((Box::from(key), Box::from(value)))
        })
        .collect::<Result<_, ThinError>>()?;

    println!("Request: {headers:#?}\n");

    let http = if req_uri == "/" {
        Ok(fs::read_to_string("./dist/index.html")?)
    } else {
        fs::read_to_string(format!("./dist/{}.html", &req_uri[1..]))
    };

    let response = match &http {
        Ok(http) => HttpResponse::with_content(HttpStatusCode::Ok, http.as_str()),
        Err(_e) => HttpResponse::new(HttpStatusCode::NotFound),
    };

    stream.write_all(response.into_string().as_bytes())?;

    Ok(())
}

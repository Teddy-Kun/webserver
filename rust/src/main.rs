use std::{
    collections::HashMap,
    fmt::Display,
    io::{BufRead, BufReader, Write},
    net::{TcpListener, TcpStream},
};

use crate::error::ThinError;

mod error;

fn main() {
    if let Err(e) = create_listener() {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}

fn create_listener() -> Result<(), ThinError> {
    let listener = TcpListener::bind("127.0.0.1:7878")?;

    for stream in listener.incoming() {
        let stream = match stream {
            Ok(s) => s,
            Err(e) => {
                eprintln!("Error reading stream: {}", e);
                continue;
            }
        };

        if let Err(e) = handle_connection(stream) {
            eprintln!("Error handling connection: {}", e);
            continue;
        }
    }

    Ok(())
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
/// https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods
enum HttpMethod {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
}

impl Display for HttpMethod {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self) // identical to debug
    }
}

// validate &str for being a valid http method
impl TryFrom<&str> for HttpMethod {
    type Error = ThinError;

    fn try_from(value: &str) -> Result<Self, Self::Error> {
        let res = match value {
            "GET" => HttpMethod::GET,
            "HEAD" => HttpMethod::HEAD,
            "POST" => HttpMethod::POST,
            "PUT" => HttpMethod::PUT,
            "DELETE" => HttpMethod::DELETE,
            "CONNECT" => HttpMethod::CONNECT,
            "OPTIONS" => HttpMethod::OPTIONS,
            "TRACE" => HttpMethod::TRACE,
            "PATCH" => HttpMethod::PATCH,
            s => return Err(ThinError::new(format!("illegal http method: {s}").into())),
        };

        Ok(res)
    }
}

fn handle_connection(mut stream: TcpStream) -> Result<(), ThinError> {
    let buf_reader = BufReader::new(&stream);

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

    if method != HttpMethod::GET {
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

    let response = "HTTP/1.1 200 OK\r\n\r\n";
    stream.write_all(response.as_bytes())?;

    Ok(())
}

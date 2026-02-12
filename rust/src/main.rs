use std::{
	error::Error,
	fs,
	io::{BufRead, BufReader, Write},
	net::{TcpListener, TcpStream},
	num::NonZeroUsize,
};

use crate::{
	http::{HttpMethod, HttpResponse, HttpStatusCode},
	thread_pool::ThreadPool,
};

mod http;
mod thread_pool;

#[cfg(feature = "dhat-heap")]
#[global_allocator]
static ALLOC: dhat::Alloc = dhat::Alloc;

fn main() {
	#[cfg(feature = "dhat-heap")]
	let _profiler = dhat::Profiler::new_heap();

	if let Err(e) = create_listener() {
		eprintln!("{}", e);
		std::process::exit(1);
	}
}

fn create_listener() -> Result<(), Box<dyn Error>> {
	let listener = TcpListener::bind("127.0.0.1:7878")?;
	let thread_pool = ThreadPool::new(NonZeroUsize::new(8).unwrap());

	#[cfg(feature = "dhat-heap")]
	let mut counter: u8 = 0;

	for stream in listener.incoming() {
		#[cfg(feature = "dhat-heap")]
		{
			counter += 1;
		}

		thread_pool.execute(move || {
			let mut stream = match stream {
				Ok(s) => s,
				Err(e) => {
					eprintln!("Error reading stream: {}", e);
					return;
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
		});

		#[cfg(feature = "dhat-heap")]
		if counter == 3 {
			break;
		}
	}

	Ok(())
}

fn handle_connection(stream: &mut TcpStream) -> Result<(), Box<dyn Error>> {
	let buf_reader = BufReader::new(&*stream);

	let mut lines = buf_reader.lines();

	let head = lines.next().ok_or(String::from("missing http head"))??;

	let mut splitter = head.split_ascii_whitespace();

	let method = splitter.next().ok_or(String::from("missing http method"))?;
	let req_uri = splitter
		.next()
		.ok_or(String::from("missing http request-uri"))?;
	let version = splitter
		.next()
		.ok_or(String::from("missing http version"))?;

	let method = HttpMethod::try_from(method)?; // validate and convert str to our enum

	println!("{method} {req_uri} {version}");

	if version != "HTTP/1.1" {
		let response = HttpResponse::new(HttpStatusCode::HttpVersionNotSupported);
		stream.write_all(response.into_string().as_bytes())?;
		return Ok(());
	}

	if method != HttpMethod::Get {
		return Err(format!("unsupported http method: {method}").into());
	}
	#[cfg(feature = "print-headers")]
	{
		use std::collections::HashMap;

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
	}

	let http = if req_uri == "/" {
		Ok(fs::read_to_string("./dist/index.html")?)
	} else {
		fs::read_to_string(format!("./dist/{}.html", &req_uri[1..]))
	};

	let response = match http {
		Ok(http) => HttpResponse::with_content(HttpStatusCode::Ok, http),
		Err(_e) => match fs::read_to_string("./dist/404.html") {
			Ok(not_found) => HttpResponse::with_content(HttpStatusCode::NotFound, not_found),
			Err(_) => HttpResponse::new(HttpStatusCode::NotFound),
		},
	};

	stream.write_all(response.into_string().as_bytes())?;

	Ok(())
}

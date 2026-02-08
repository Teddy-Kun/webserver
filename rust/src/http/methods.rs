use std::{error::Error, fmt::Display};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
/// https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods
pub enum HttpMethod {
	Get,
	Head,
	Post,
	Put,
	Delete,
	Connect,
	Options,
	Trace,
	Patch,
}

impl Display for HttpMethod {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		write!(f, "{:?}", self) // identical to debug
	}
}

// validate &str for being a valid http method
impl TryFrom<&str> for HttpMethod {
	type Error = Box<dyn Error>;

	fn try_from(value: &str) -> Result<Self, Self::Error> {
		let res = match value {
			"GET" => HttpMethod::Get,
			"HEAD" => HttpMethod::Head,
			"POST" => HttpMethod::Post,
			"PUT" => HttpMethod::Put,
			"DELETE" => HttpMethod::Delete,
			"CONNECT" => HttpMethod::Connect,
			"OPTIONS" => HttpMethod::Options,
			"TRACE" => HttpMethod::Trace,
			"PATCH" => HttpMethod::Patch,
			s => return Err(format!("illegal http method: {s}").into()),
		};

		Ok(res)
	}
}

use std::fmt::Display;

use crate::error::ThinError;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
/// https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Methods
pub enum HttpMethod {
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

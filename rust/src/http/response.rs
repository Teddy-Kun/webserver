use std::fmt::Display;

#[derive(Debug, Clone, Copy)]
#[repr(u16)]
/// https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status
pub enum HttpStatusCode {
    Continue = 100,
    SwitchingProtocols = 101,
    Processing = 102,
    EarlyHints = 103,

    Ok = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultiStatus = 207,
    AlreadyReported = 208,
    IMUsed = 226,

    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    #[deprecated = "Defined in a previous version of the HTTP specification to indicate that a requested response must be accessed by a proxy. It has been deprecated due to security concerns regarding in-band configuration of a proxy."]
    UseProxy = 305,
    // Unused = 306,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    LengthRequired = 411,
    PreconditionFailed = 412,
    ContentTooLarge = 413,
    UriTooLong = 414,
    UnsupportedMediaType = 415,
    RangeNotSatisfiable = 416,
    ExpectationFailed = 417,
    ImATeapot = 418,
    MisdirectedRequest = 421,
    UnprocessableContent = 422,
    Locked = 423,
    FailedDependency = 424,
    TooEarly = 425,
    UpgradeRequired = 426,
    PreconditionRequired = 428,
    TooManyRequests = 429,
    RequestHeaderFieldsTooLarge = 431,
    UnavailableForLegalReason = 451,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505,
    VariantAlsoNegotiates = 506,
    InsufficientStorage = 507,
    LoopDetected = 508,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511,
}

impl Display for HttpStatusCode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let s = match *self {
            HttpStatusCode::Continue => "Continue",
            HttpStatusCode::SwitchingProtocols => "Switching Protocols",
            HttpStatusCode::Processing => "Processing",
            HttpStatusCode::EarlyHints => "Early Hints",
            HttpStatusCode::Ok => "OK",
            HttpStatusCode::Created => "Created",
            HttpStatusCode::Accepted => "Accepted",
            HttpStatusCode::NonAuthoritativeInformation => "Non-Authoritative Information",
            HttpStatusCode::NoContent => "No Content",
            HttpStatusCode::ResetContent => "Reset Content",
            HttpStatusCode::PartialContent => "Partial Content",
            HttpStatusCode::MultiStatus => "Multi-Status",
            HttpStatusCode::AlreadyReported => "AlreadyReported",
            HttpStatusCode::IMUsed => "IM Used",
            HttpStatusCode::MultipleChoices => "Multiple Choices",
            HttpStatusCode::MovedPermanently => "Moved Permanently",
            HttpStatusCode::Found => "Found",
            HttpStatusCode::SeeOther => "See Other",
            HttpStatusCode::NotModified => "Not Modified",
            HttpStatusCode::UseProxy => "Use Proxy",
            HttpStatusCode::TemporaryRedirect => "Temporary Redirect",
            HttpStatusCode::PermanentRedirect => "Permanent Redirect",
            HttpStatusCode::BadRequest => "Bad Request",
            HttpStatusCode::Unauthorized => "Unauthorized",
            HttpStatusCode::PaymentRequired => "Payment Required",
            HttpStatusCode::Forbidden => "Forbidden",
            HttpStatusCode::NotFound => "NotFound",
            HttpStatusCode::MethodNotAllowed => "Method Not Allowed",
            HttpStatusCode::NotAcceptable => "Not Acceptable",
            HttpStatusCode::ProxyAuthenticationRequired => "Proxy Authentication Required",
            HttpStatusCode::RequestTimeout => "Request Timeout",
            HttpStatusCode::Conflict => "Conflict",
            HttpStatusCode::Gone => "Gone",
            HttpStatusCode::LengthRequired => "Length Required",
            HttpStatusCode::PreconditionFailed => "Precondition Failed",
            HttpStatusCode::ContentTooLarge => "Content Too Large",
            HttpStatusCode::UriTooLong => "URI Too Long",
            HttpStatusCode::UnsupportedMediaType => "UnsupportedMediaType",
            HttpStatusCode::RangeNotSatisfiable => "Range Not Satisfiable",
            HttpStatusCode::ExpectationFailed => "Expectation Failed",
            HttpStatusCode::ImATeapot => "I'm a teapot",
            HttpStatusCode::MisdirectedRequest => "Misdirected Request",
            HttpStatusCode::UnprocessableContent => "Unprocessable Content",
            HttpStatusCode::Locked => "Locked",
            HttpStatusCode::FailedDependency => "Failed Dependency",
            HttpStatusCode::TooEarly => "Too Early",
            HttpStatusCode::UpgradeRequired => "Upgrade Required",
            HttpStatusCode::PreconditionRequired => "Precondition Required",
            HttpStatusCode::TooManyRequests => "Too Many Requests",
            HttpStatusCode::RequestHeaderFieldsTooLarge => "Request Header Fields Too Large",
            HttpStatusCode::UnavailableForLegalReason => "Unavailable For Legal Reasons",
            HttpStatusCode::InternalServerError => "Internal Server Error",
            HttpStatusCode::NotImplemented => "Not Implemented",
            HttpStatusCode::BadGateway => "Bad Gateway",
            HttpStatusCode::ServiceUnavailable => "Service Unavailable",
            HttpStatusCode::GatewayTimeout => "Gateway Timeout",
            HttpStatusCode::HttpVersionNotSupported => "HTTP Version Not Supported",
            HttpStatusCode::VariantAlsoNegotiates => "Variant Also Negotiates",
            HttpStatusCode::InsufficientStorage => "Insufficient Storage",
            HttpStatusCode::LoopDetected => "Loop Detected",
            HttpStatusCode::NotExtended => "Not Extended",
            HttpStatusCode::NetworkAuthenticationRequired => "Network Authentication Required",
        };

        write!(f, "{} {}", *self as u16, s)
    }
}

#[derive(Debug)]
pub struct HttpResponse<'r> {
    status: HttpStatusCode,
    content: Option<&'r str>,
}

impl<'r> HttpResponse<'r> {
    pub fn new(status: HttpStatusCode) -> Self {
        Self {
            status,
            content: None,
        }
    }

    pub fn with_content(status: HttpStatusCode, content: &'r str) -> Self {
        Self {
            status,
            content: Some(content),
        }
    }

    pub fn into_string(self) -> String {
        self.into()
    }
}

impl<'r> Into<String> for HttpResponse<'r> {
    fn into(self) -> String {
        match self.content {
            None => format!("HTTP/1.1 {}\r\n\r\n", self.status),
            Some(content) => format!(
                "HTTP/1.1 {}\r\nContent-Length: {}\r\n\r\n{}",
                self.status,
                content.len(),
                content
            ),
        }
    }
}

use std::{
    error::Error,
    fmt::{Debug, Display},
};

/// A `dyn Error` wrapped in two Boxes, to reduce stack size from 16 to 8.
/// Very crude and less optimal than `anyhow::Error`, but its simple and I don't want to use any crates.
pub struct ThinError {
    inner: Box<Box<dyn Error>>,
}

impl ThinError {
    pub fn new(e: Box<dyn Error>) -> Self {
        Self { inner: Box::new(e) }
    }

    pub fn str(s: &str) -> Self {
        Self {
            inner: Box::new(s.into()),
        }
    }
}

impl Error for ThinError {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        Some((*self.inner).as_ref())
    }
}

impl Debug for ThinError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", *self.inner)
    }
}

impl Display for ThinError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", *self.inner)
    }
}

impl From<std::io::Error> for ThinError {
    fn from(value: std::io::Error) -> Self {
        Self::new(value.into())
    }
}

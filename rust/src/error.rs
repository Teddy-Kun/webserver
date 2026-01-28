use std::{
	error::Error,
	fmt::{Debug, Display},
};

pub struct ThinError {
	inner: Box<dyn Error>,
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
		Some(self.inner.as_ref())
	}
}

impl Debug for ThinError {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		write!(f, "{:?}", self.inner)
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

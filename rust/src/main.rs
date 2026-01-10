use std::net::TcpListener;

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

        println!("Connection established!");
    }

    Ok(())
}

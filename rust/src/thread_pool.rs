use std::{
    num::NonZeroUsize,
    sync::{Arc, Mutex, mpsc},
    thread::{self, JoinHandle},
};

pub struct ThreadPool {
    workers: Vec<Worker>,
    sender: mpsc::Sender<Job>,
}

impl ThreadPool {
    pub fn new(size: NonZeroUsize) -> Self {
        let mut workers = Vec::with_capacity(size.get());

        let (sender, receiver) = mpsc::channel();

        let receiver = Arc::new(Mutex::new(receiver));

        for id in 0..size.get() {
            let clone = receiver.clone();
            workers.push(Worker::new(id, clone));
        }

        println!("{size} worker threads created");

        Self { workers, sender }
    }

    pub fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        let job = Box::new(f);
        self.sender.send(job).expect("receiver is gone")
    }
}

struct Worker {
    id: usize,
    thread: JoinHandle<()>,
}

impl Worker {
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Job>>>) -> Self {
        let thread = thread::spawn(move || {
            loop {
                let job = receiver
                    .lock()
                    .expect("receiver mutex is poisoned")
                    .recv()
                    .expect("sender is gone");
                println!("Worker {id}, got a job; executing");
                job();
            }
        });
        Self { id, thread }
    }
}

type Job = Box<dyn FnOnce() + Send + 'static>;

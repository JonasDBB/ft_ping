FROM debian:11

RUN apt-get update
RUN apt-get install -y gcc cmake make g++ gdb
##
# Dockerfile for building criu-dsm in a docker container.
#
# This file builds the criu-dsm using the Ubuntu 22.04 as the base image.
#
# docker build -t ubuntu2204:criu .
# docker run --rm -v $(pwd)/criu-3.15:/code --user $(id -u):$(id -g) \
#	ubuntu2204:criu make -C /code
##

FROM --platform=linux/amd64 ubuntu:jammy

RUN apt-get update -y && apt install -y --no-install-recommends \
	libprotobuf-dev libprotobuf-c-dev protobuf-c-compiler \
	protobuf-compiler python3-protobuf \
	libcap-dev libnl-3-dev libnet-dev libbsd-dev pkg-config \
	python3-future libnet1-dev libnftables-dev libgnutls28-dev \
	gcc make

COPY . ./code

RUN make -C /code -j4

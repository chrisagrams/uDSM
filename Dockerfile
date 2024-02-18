##
# Dockerfile for criu-dsm
#
# This file builds the criu-dsm using the Ubuntu 22.04 as the base image.
#
# Build application code (located in ./code):
# docker run --rm -v $(pwd)/app:/code/app <image id> make -C /code/app
##

FROM ubuntu:jammy

#ARG BRANCH_URL="https://github.com/ssrg-vt/popcorn-compiler"
#ARG BRANCH_NAME=main

#ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && apt install -y --no-install-recommends \
	libprotobuf-dev libprotobuf-c-dev protobuf-c-compiler \
	protobuf-compiler python3-protobuf \
	libcap-dev libnl-3-dev libnet-dev libbsd-dev pkg-config \
	python3-future libnet1-dev libnftables-dev libgnutls28-dev \
	gcc make

#WORKDIR /code
#RUN git clone ${BRANCH_URL} -b ${BRANCH_NAME} --depth 1

#WORKDIR /code/popcorn-compiler
#RUN ./install_compiler.py \
#      --install-all \
#      --with-popcorn-kernel-5_2 \
#      --libmigration-type=signal_trigger && \
#      rm -rf /usr/local/popcorn/src

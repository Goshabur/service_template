# build
FROM ghcr.io/userver-framework/docker-userver-build-base:v1a as build-stage

WORKDIR /OneAndOnly_build

COPY . /OneAndOnly_build

RUN make build-release

# production
FROM debian:bullseye-slim as production-stage
#FROM ubuntu:latest

RUN echo "Europe/Moscow" > /etc/timezone
RUN ln -sf /usr/share/zoneinfo/Europe/Moscow /etc/localtime
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update

RUN apt-get install -y --allow-unauthenticated \
    binutils-dev \
    build-essential \
    ccache \
	libjemalloc-dev \
	libyaml-cpp-dev \
	libfmt-dev \
	libc-ares-dev \
	libcctz-dev \
	curl \
	libcurl4-openssl-dev \
	libboost-program-options1.74-dev \
	libboost-filesystem1.74-dev \
	libboost-regex1.74-dev \
	libboost-iostreams1.74-dev \
	libhttp-parser-dev \
	libev-dev \
	libldap2-dev \
	libssl-dev \
	zlib1g-dev \
	libkrb5-dev \
	libpq-dev \
	postgresql-server-dev-13 \
	libspdlog-dev \
	postgresql-13 \
	chrpath \
	vim \
	sudo \
	gnupg2 \
	wget \
	dirmngr

WORKDIR /OneAndOnly

RUN mkdir -p bin configs

COPY --from=build-stage /OneAndOnly_build/build_release/cpp-reseller-backend bin/

ENV PATH /usr/sbin:/usr/bin:/sbin:/bin:/usr/lib/postgresql/13/bin:/OneAndOnly/bin:${PATH}

CMD ["cpp-reseller-backend", "--config", "/OneAndOnly/configs/static_config.yaml"]


FROM alpine:3.10

WORKDIR /usr/src/app/xv6-public

COPY xv6-public/ .

RUN apk --no-cache add \
        build-base \
        gcc \
        g++ \
        libc-dev \
        make \
        git \
        bash \
        perl \
        qemu-system-x86_64 \
    && make

ENTRYPOINT ["make", "qemu"]


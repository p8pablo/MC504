FROM alpine:3.18

WORKDIR /usr/src/app

COPY . .

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


ENTRYPOINT [ "make", "qemu" ]

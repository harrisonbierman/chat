FROM alpine:3.18 AS builder

RUN apk add --no-cache gcc musl-dev build-base net-tools

WORKDIR /app

COPY client.c .

# RUN is used at build time
RUN gcc -o client client.c 


FROM alpine:3.18 

WORKDIR /app

RUN adduser -D client

USER client

COPY --from=builder /app/client .

EXPOSE 65432

# CMD is used after build time
CMD ["./client"] 

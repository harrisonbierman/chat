# Stage 1: Build 
FROM alpine:3.18 AS builder

# Install dependencies (gcc, musl-dev, make)
RUN apk add --no-cache gcc musl-dev build-base net-tools

WORKDIR /app

COPY server.c .

RUN gcc server.c -o server


# Stage 2: Runtime
FROM alpine:3.18

#create non-root user and group
RUN adduser -D server

WORKDIR /app

COPY --from=builder /app/server .

# Expose port
EXPOSE 65432

USER server

CMD ["./server"]

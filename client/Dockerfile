FROM archlinux:latest AS builder

RUN pacman -Syu --noconfirm gcc base-devel net-tools git
RUN pacman -S --noconfirm alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama

WORKDIR /app

COPY client.c .

# RUN is used at build time
RUN gcc -o client client.c 


FROM archlinux:latest 

WORKDIR /app

RUN useradd -m client

USER client

COPY --from=builder /app/client .

EXPOSE 65432

# CMD is used after build time
CMD ["./client"] 

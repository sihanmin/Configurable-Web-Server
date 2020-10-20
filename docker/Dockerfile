### Build/test container ###
# Define builder stage
FROM aptx:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure



### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

RUN apt-get update && apt-get install -y curl

# TODO(aptx): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/tests/docker_config .
COPY --from=builder /usr/src/project/static ./static
COPY --from=builder /usr/src/project/static2 ./static2
COPY --from=builder /usr/src/project/t_rex ./t_rex

# TODO(aptx): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(aptx): Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(aptx): Update with real server args
CMD ["docker_config"]

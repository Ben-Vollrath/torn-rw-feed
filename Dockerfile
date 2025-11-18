FROM debian:bookworm-slim AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates libpq5 libssl3 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/local/bin
# CI must place the built binary at release/torn_rw_feed in the build context
COPY release/torn_rw_feed /usr/local/bin/torn_rw_feed
RUN chmod +x /usr/local/bin/torn_rw_feed

EXPOSE 8000
ENTRYPOINT ["/usr/local/bin/torn_rw_feed"]
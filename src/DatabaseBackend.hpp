#pragma once

// Toggle via build flag: -DUSE_SQLITE=ON (tests) vs OFF (prod)
#ifdef USE_SQLITE
  #include "oatpp-sqlite/orm.hpp"
  namespace dbb {
    using ConnProvider
    using ConnPool
    using Executor
    inline std::string connString() {
      // in-memory shared DB for tests;
      return "file::memory:?cache=shared";
    }
  }
#else
  #include "oatpp-postgresql/orm.hpp"
  namespace dbb {
    using ConnProvider = oatpp::postgresql::ConnectionProvider;
    using ConnPool     = oatpp::postgresql::ConnectionPool;
    using Executor     = oatpp::postgresql::Executor;
    inline std::string connString() {
      return "postgresql://torn:tornpass@192.168.0.117:5432/torn_rw_feed";
    }
  }
#endif

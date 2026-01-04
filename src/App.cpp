#include <sentry.h>

#include <iostream>

#include "ApiErrorHandler.hpp"
#include "AppComponent.hpp"
#include "controller/AuthController.hpp"
#include "controller/UserController.hpp"
#include "controller/WarController.hpp"
#include "oatpp-swagger/AsyncController.hpp"
#include "oatpp/network/Server.hpp"

void run() {
  /* Register Components in scope of run() method */
  AppComponent components;

  /* Get router component */
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

  auto authController = AuthController::createShared();
  auto warController = WarController::createShared();
  auto userController = UserController::createShared();
  authController->setErrorHandler(std::make_shared<ApiErrorHandler>());
  /* Add all controllers */
  router->addController(authController);
  router->addController(warController);
  router->addController(userController);

  oatpp::web::server::api::Endpoints docEndpoints;
  docEndpoints.append(authController->getEndpoints());
  docEndpoints.append(warController->getEndpoints());
  docEndpoints.append(userController->getEndpoints());

  router->addController(
      oatpp::swagger::AsyncController::createShared(docEndpoints));

  /* Get connection handler component */
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                  serverConnectionHandler);

  /* Get connection provider component */
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>,
                  connectionProvider);

  /* Create server which takes provided TCP connections and passes them to HTTP
   * connection handler */
  oatpp::network::Server server(connectionProvider, serverConnectionHandler);

  /* Print info about server port */
  OATPP_LOGI("MyApp", "Server running on port %s",
             connectionProvider->getProperty("port").getData())

  /* Run server */
  server.run();
}

/**
 *  main
 */
int main(int argc, const char* argv[]) {
  sentry_options_t* options = sentry_options_new();
  sentry_init(options);

  sentry_capture_event(sentry_value_new_message_event(
      /*   level */ SENTRY_LEVEL_INFO,
      /*  logger */ "custom",
      /* message */ "Started Service"));

  oatpp::base::Environment::init();

  run();

  /* Print how much objects were created during app running, and what have
   * left-probably leaked */
  /* Disable object counting for release builds using '-D
   * OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount()
            << "\n";
  std::cout << "objectsCreated = "
            << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  oatpp::base::Environment::destroy();
  sentry_close();

  return 0;
}

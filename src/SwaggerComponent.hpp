#pragma once

#include "oatpp/core/macro/component.hpp"
#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"

/**
 *  Swagger ui is served at
 *  http://host:port/swagger/ui
 */
class SwaggerComponent
{
public:
	/**
	 *  General API docs info
	 */
	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swaggerDocumentInfo)([]
	{
		oatpp::swagger::DocumentInfo::Builder builder;

		builder
			.setTitle("Torn RW Feed")
			.setDescription("")
			.setVersion("1.0")
			.addServer("https://localhost:8000", "server on localhost")
			.addSecurityScheme("basic_auth",
			                   oatpp::swagger::DocumentInfo::SecuritySchemeBuilder::DefaultBearerAuthorizationSecurityScheme(
				                   "Authorization"));

		return builder.build();
	}());


	OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swaggerResources)([]
	{
		OATPP_COMPONENT(std::shared_ptr<AppConfig>, appConfig);
		return oatpp::swagger::Resources::loadResources(appConfig->oatppSwaggerResPath);
	}());
};

#include "view.hpp"

#include <fmt/format.h>

#include <postgresql/libpq/libpq-fs.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <vector>

namespace cpp_reseller_backend {

    namespace {

        class InsertData final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view
            kName = "handler-insert-data";

            InsertData(const userver::components::ComponentConfig &config,
                    const userver::components::ComponentContext &component_context)
                    : HttpHandlerBase(config, component_context),
                      pg_cluster_(
                              component_context
                                      .FindComponent<userver::components::Postgres>("postgres-db-1")
                                      .GetCluster()) {}

            std::string HandleRequestThrow(
                    const userver::server::http::HttpRequest &request,
                    userver::server::request::RequestContext &) const override {
                auto request_body = userver::formats::json::FromString(request.RequestBody());
                /// request type [model, memory, color, country, modified = new]

                auto model = request_body["model"].As < std::optional < std::string >> ();
                auto memory = request_body["memory"].As < std::optional < std::string >> ();
                auto color = request_body["color"].As < std::optional < int >> ();
                auto country = request_body["country"].As < std::optional < std::string >> ();
                auto modified = request_body["modified"].As < std::optional < std::string >> ();


                return "Hello world !";
            }

            userver::storages::postgres::ClusterPtr pg_cluster_;
        };


        const userver::storages::postgres::Query kSelectOnlyByModel{
                "SELECT FROM one_and_only.iphone WHERE model = ANY($1)",
                userver::storages::postgres::Query::Name{"select_by_model"},
        };

        const userver::storages::postgres::Query kSelectByModelAndMemory{
                "SELECT FROM one_and_only.iphone WHERE (model = ANY($1) AND memory = ANY($2))",
                userver::storages::postgres::Query::Name{"select_by_model"},
        };

        const userver::storages::postgres::Query kSelectByModelMemoryColor{
                "SELECT FROM one_and_only.iphone WHERE (model = ANY($1) AND memory = ANY($2) AND color = ANY($3))",
                userver::storages::postgres::Query::Name{"select_by_model"},
        };


    } // namespace

    void AppendInsertData(userver::components::ComponentList &component_list) {
        component_list.Append<InsertData>();
    }

} // namespace cpp-reseller-backend

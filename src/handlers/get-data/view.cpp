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
#include <userver/formats/json.hpp>
#include <vector>
#include <userver/formats/serialize/common_containers.hpp>

#include "../../model/iphone.hpp"
#include "../../model/supplier_iphone.hpp"

namespace cpp_reseller_backend {

    namespace {

//        const userver::storages::postgres::Query kSelectByModelMemoryColor{
//                "SELECT bot_user_id, iphone_id, country, CAST(price_amount as varchar(30)), price_currency, CAST(modified_at as varchar(30)) "
//                "FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) ) "
//                "ORDER BY price_amount ASC LIMIT 5",
//                userver::storages::postgres::Query::Name{"select_by_model_memory_color"},
//        };



        const userver::storages::postgres::Query kSelectByModelMemoryColor{
                "SELECT t.bot_user_id, t.iphone_id, t.country, CAST(t.price_amount as varchar(30)), t.price_currency, CAST(t.modified_at as varchar(30)) FROM (\n"
                "        (SELECT t1.bot_user_id, t1.iphone_id, t1.country, t1.price_amount, t1.price_currency, t1.modified_at FROM\n"
                "            (SELECT bot_user_id, iphone_id, country, price_amount, price_currency, modified_at FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as t1\n"
                "            INNER JOIN(\n"
                "                SELECT t2.bot_user_id, t2.iphone_id, t2.country, t2.price_amount, t2.price_currency, t2.modified_at FROM\n"
                "                (SELECT bot_user_id, iphone_id, country, price_amount, price_currency, modified_at FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as t2\n"
                "                WHERE 2=(\n"
                "                    SELECT COUNT(DISTINCT(t3.price_amount)) FROM\n"
                "                    (SELECT bot_user_id, iphone_id, country, price_amount, price_currency, modified_at FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as t3\n"
                "                    WHERE t3.price_amount>t2.price_amount AND t2.country=t3.country\n"
                "                )\n"
                "            ) t4\n"
                "        ON t1.country=t4.country WHERE t1.price_amount>=t4.price_amount)\n"
                "        UNION ALL(\n"
                "            SELECT t5.bot_user_id, t5.iphone_id, t5.country, t5.price_amount, t5.price_currency, t5.modified_at FROM\n"
                "            (SELECT bot_user_id, iphone_id, country, price_amount, price_currency, modified_at FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as t5\n"
                "            WHERE t5.country IN (SELECT t6.country FROM\n"
                "                (SELECT bot_user_id, iphone_id, country, price_amount, price_currency, modified_at FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as t6 GROUP BY t6.country HAVING COUNT(1)<3)\n"
                "        )\n"
                ") t\n"
                "ORDER BY t.country ASC,t.price_amount DESC;",
                userver::storages::postgres::Query::Name{"select_by_model_memory_color"},
        };


        const userver::storages::postgres::Query kSelectByModelMemoryColorCountry{
                "SELECT * FROM (SELECT bot_user_id, iphone_id, country, CAST(price_amount as varchar(30)), price_currency, CAST(modified_at as varchar(30)) "
                "FROM supplier_iphone WHERE iphone_id = ANY( SELECT id FROM iphone WHERE (model = $1 AND memory = $2 AND color = $3) )) as sub WHERE sub.country = $4"
                "ORDER BY sub.price_amount ASC LIMIT 5",
                userver::storages::postgres::Query::Name{"select_by_model_memory_color_country"},
        };

        class GetData final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view
            kName = "handler-get-data";

            GetData(const userver::components::ComponentConfig &config,
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
                auto color = request_body["color"].As < std::optional < std::string >> ();
                auto country = request_body["country"].As < std::optional < std::string >> ();


                userver::formats::json::ValueBuilder response;
                if(!country.has_value()) {
                    auto result = pg_cluster_->Execute(
                            userver::storages::postgres::ClusterHostType::kSlave,
                            kSelectByModelMemoryColor, model, memory, color);
//                    for(auto row: result.AsSetOf<TSupplier_Iphone>(userver::storages::postgres::kRowTag)) {
//                        response["items"].PushBack(row);
//                    }
//                    return ToString(response.ExtractValue());
                    return "NO COUNTRY !";
                }else{
                    auto result = pg_cluster_->Execute(
                            userver::storages::postgres::ClusterHostType::kSlave,
                            kSelectByModelMemoryColorCountry, model, memory, color, country);
                    for(auto row: result.AsSetOf<TSupplier_Iphone>(userver::storages::postgres::kRowTag)) {
                        response["items"].PushBack(row);
                    }
//                    return ToString(response.ExtractValue());
                    return "WE HAVE COUNTRY !";
                }
                return "hello !";
            }

            userver::storages::postgres::ClusterPtr pg_cluster_;
        };
    } // namespace

    void AppendGetData(userver::components::ComponentList &component_list) {
        component_list.Append<GetData>();
    }
} // namespace cpp-reseller-backend
//
//curl -v -X GET \
//  -H "Content-type: application/json" \
//  -H "Accept: application/json" \
//  -d '{"model":"IPHONE_14_PRO_MAX", "memory":"GB_512", "color":"SILVER"}' \
//  "http://localhost:8080/get-data"

//curl -v -X GET \
//  -H "Content-type: application/json" \
//  -H "Accept: application/json" \
//  -d '{"model":"IPHONE_14_PRO_MAX", "memory":"GB_512", "color":"SILVER", "country":"JAPAN"}' \
//  "http://localhost:8080/get-data"


//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"CANADA","price_amount":"68000.0000","price_currency":"RUB","modified_at":"2023-01-08 10:06:22.761898"},
//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"CHINA","price_amount":"68500.0000","price_currency":"RUB","modified_at":"2023-01-08 10:28:49.356114"},
//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"GREAT_BRITAIN","price_amount":"66000.0000","price_currency":"RUB","modified_at":"2023-01-08 10:25:53.111862"},
//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"JAPAN","price_amount":"68000.5000","price_currency":"RUB","modified_at":"2023-01-08 01:17:29"},
//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"Russia","price_amount":"67000.0000","price_currency":"RUB","modified_at":"2023-01-08 10:25:53.827094"},
//{"bot_user_id":"telegram_9cb55bc2-9208-40f4-8abe-caa033359439","iphone_id":"IPHONE_14_PRO_MAX/GB_512/SILVER","country":"USA","price_amount":"69500.5000","price_currency":"RUB","modified_at":"2023-01-08 00:57:04"}






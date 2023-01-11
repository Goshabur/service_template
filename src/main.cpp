#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/clients/dns/component.hpp>


#include "handlers/get-data/view.hpp"
#include "handlers/delete-data/view.hpp"
#include "handlers/insert-data/view.hpp"

int main(int argc, char *argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>()
                            .Append<userver::components::Postgres>("postgres-db-1")
                            .Append<userver::clients::dns::Component>();


    cpp_reseller_backend::AppendGetData(component_list);
    cpp_reseller_backend::AppendDeleteData(component_list);
    cpp_reseller_backend::AppendInsertData(component_list);

    return userver::utils::DaemonMain(argc, argv, component_list);
}

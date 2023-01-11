#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>


namespace cpp_reseller_backend {

    struct TSupplier_Iphone {
        std::string bot_user_id;
        std::string iphone_id;
        std::string country;
        std::string price_amount;
        std::string price_currency;
        std::string modified_at;
    };

    userver::formats::json::Value Serialize(const TSupplier_Iphone& data,
                                            userver::formats::serialize::To<userver::formats::json::Value>);
}
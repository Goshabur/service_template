#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>


namespace cpp_reseller_backend {

    struct TIphone {
        std::string id;
//        std::string user_id;
        std::string model;
        std::string memory_gb;
        std::string color;
//        std::string country;
//        std::string price_amount;
//        std::string price_currency;
//        std::string modified_at;
    };

    userver::formats::json::Value Serialize(const TIphone& data,
                                            userver::formats::serialize::To<userver::formats::json::Value>);
}
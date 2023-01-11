#include "iphone.hpp"

namespace cpp_reseller_backend {

    userver::formats::json::Value Serialize(const TIphone& iphone,
                                            userver::formats::serialize::To<userver::formats::json::Value>) {
        userver::formats::json::ValueBuilder item;
        item["id"] = iphone.id;
//        item["model"] = iphone.model;
//        item["memory_gb"] = iphone.memory_gb;
//        item["color"] = iphone.color;
//        item["title"] = bookmark.title;
//        item["created_ts"] = bookmark.created_ts;
        return item.ExtractValue();
    }

}  // namespace cpp_reseller_backend
#include "supplier_iphone.hpp"

namespace cpp_reseller_backend {

    userver::formats::json::Value Serialize(const TSupplier_Iphone& iphone,
                                            userver::formats::serialize::To<userver::formats::json::Value>) {
        userver::formats::json::ValueBuilder item;
        item["bot_user_id"] = iphone.bot_user_id;
        item["iphone_id"] = iphone.iphone_id;
        item["country"] = iphone.country;
        item["price_amount"] = iphone.price_amount;
        item["price_currency"] = iphone.price_currency;
        item["modified_at"] = iphone.modified_at;
        return item.ExtractValue();
    }

}  // namespace cpp_reseller_backend
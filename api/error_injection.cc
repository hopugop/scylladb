/*
 * Copyright (C) 2020-present ScyllaDB
 */

/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "api/api-doc/error_injection.json.hh"
#include "api/api.hh"

#include <seastar/http/exception.hh>
#include "log.hh"
#include "utils/error_injection.hh"
#include <seastar/core/future-util.hh>

namespace api {

namespace hf = httpd::error_injection_json;

void set_error_injection(http_context& ctx, routes& r) {

    hf::enable_injection.set(r, [](std::unique_ptr<request> req) {
        sstring injection = req->param["injection"];
        bool one_shot = req->get_query_param("one_shot") == "True";
        auto& errinj = utils::get_local_injector();
        return errinj.enable_on_all(injection, one_shot).then([] {
            return make_ready_future<json::json_return_type>(json::json_void());
        });
    });

    hf::get_enabled_injections_on_all.set(r, [](std::unique_ptr<request> req) {
        auto& errinj = utils::get_local_injector();
        auto ret = errinj.enabled_injections_on_all();
        return make_ready_future<json::json_return_type>(ret);
    });

    hf::disable_injection.set(r, [](std::unique_ptr<request> req) {
        sstring injection = req->param["injection"];

        auto& errinj = utils::get_local_injector();
        return errinj.disable_on_all(injection).then([] {
            return make_ready_future<json::json_return_type>(json::json_void());
        });
    });

    hf::disable_on_all.set(r, [](std::unique_ptr<request> req) {
        auto& errinj = utils::get_local_injector();
        return errinj.disable_on_all().then([] {
            return make_ready_future<json::json_return_type>(json::json_void());
        });
    });

}

} // namespace api

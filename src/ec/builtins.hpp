#pragma once

#ifndef _ec_builtins_hpp
#define _ec_builtins_hpp

#include "../common/term.hpp"
#include "../interp/interpreter.hpp"

namespace prologcoin { namespace ec {

class builtins {
public:
    using term = prologcoin::common::term;
    using interpreter_base = prologcoin::interp::interpreter_base;

    static void load(interpreter_base &interp);

    // privkey(X) true iff X is a private key.
    // Can be used to generate new private keys.
    static bool privkey_1(interpreter_base &interp, size_t arity, term args[] );
    // pubkey(X, Y) true iff Y is public key of private key X.
    // Can also be used to generate the public key Y from X.
    static bool pubkey_2(interpreter_base &interp, size_t arity, term args[] );

    // address(X, Y) true iff Y is the bitcoin address of public key X.
    // Can also be used to generate the address Y from the public key X.
    static bool address_2(interpreter_base &interp, size_t arity, term args[] );
    // sign(X, Data, Signture) true iff Signature is the obtained signature for
    // signing Data using X. If Signature is provided (not a variable), then
    // the same predicate can be used to verify the signature, but then
    // X is the public key.
    static bool sign_3(interpreter_base &interp, size_t arity, term args[] );

private:
    static bool get_private_key(interpreter_base &interp, term big0, uint8_t rawkey[32]);
    static bool get_public_key(interpreter_base &interp, term big0, uint8_t rawkey[32]);
    static bool get_address(uint8_t pubkey[32], uint8_t address[20]);
    static bool compute_signature(interpreter_base &interp, const term data,
				  const term privkey, term &out_signature);
    static bool get_hashed_data(interpreter_base &interp, const term data,
				uint8_t hash[32]);

};

}}

#endif


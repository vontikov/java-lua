package com.githhub.vontikov.jl;

/**
 * The values of this enum correspond to the Lua return codes.
 *
 * @see https://www.lua.org/docs.html for the details.
 */
public enum LuaRetCode {

    LUA_OK(0),
    LUA_YIELD(1),
    LUA_ERRRUN(2),
    LUA_ERRSYNTAX(3),
    LUA_ERRMEM(4),
    LUA_ERRERR(5);

    private final int code;

    public static LuaRetCode get(final int code) {
        for (var v : values()) {
            if (v.code == code) {
                return v;
            }
        }
        throw new RuntimeException(String.format("Lua return code not found: %d", code));
    }

    private LuaRetCode(final int code) {
        this.code = code;
    }
}

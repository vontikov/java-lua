package com.githhub.vontikov.jl;

import static org.junit.jupiter.api.Assertions.*;

import java.util.*;

import org.junit.jupiter.api.*;

class LuaEngineTest {

    @Test
    void shouldCallExternalFunctionWithArgs() {
        final String script =
                "r1 = java:func(42, 'xyz')\n" +
                "print('call result: ', r1) -- 100\n" +
                "r2 = ns1:func('abc', 42, true, false)\n" +
                "print('call result: ', r2) -- 200\n";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.setLogLevel("trace");

                // register external function in default namespace
                e.registerFunction("func", (Object... args) -> {

                    assertEquals(2, args.length);
                    assertEquals(42.0, args[0]);
                    assertEquals("xyz", args[1]);

                    return 100; // result to script
                });

                // register external function in custom namespace
                e.registerFunction("ns1", "func", (Object... args) -> {

                    assertEquals(4, args.length);
                    assertEquals("abc", args[0]);
                    assertEquals(42.0, args[1]);
                    assertEquals(true, args[2]);
                    assertEquals(false, args[3]);

                    return 200; // result to script
                });

                e.execute(script);
            }
        });
    }

    @Test
    void shouldCallExternalFunctionWithoutArgs() {
        final String script =
                "r = ns:fn()\n" +
                "print('call result: ', r) -- 5";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.registerFunction("ns", "fn", (Object... args) -> {
                    assertEquals(0, args.length);
                    return 5;
                });
                e.execute(script);
            }
        });
    }

    @Test
    void shouldCallExternalFunctionsInOneNamespace() {
        final String script =
                "r = ns:fn1()\n" +
                "print('call result: ', r) -- 5\n" +
                "r = ns:fn2()\n" +
                "print('call result: ', r) -- 10\n";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.setLogLevel("trace");

                e.registerFunction("ns", "fn1", (Object... args) -> {
                    assertEquals(0, args.length);
                    return 5;
                });
                e.registerFunction("ns", "fn2", (Object... args) -> {
                    assertEquals(0, args.length);
                    return 10;
                });
                e.execute(script);
            }
        });
    }

    @Test
    void shouldCallSeveralExternalFunctionMultipleTimes() {
        final String script =
                "s = 'abc'\n" +
                "n = 42\n" +
                "b = true\n" +

                "r = java:fn(s)\n" +
                "print('call result: ', r)\n" +

                "r = ns:fn1(n)\n" +
                "print('call result: ', r)\n" +

                "r = ns:fn2(b)\n" +
                "print('call result: ', r)\n";

        final int max = 10;
        final List<String> strings = new ArrayList<>();
        final List<Double> doubles = new ArrayList<>();
        final List<Boolean> booleans = new ArrayList<>();

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.registerFunction("fn", (Object... args) -> {
                    assertTrue(args[0] instanceof String);
                    assertEquals("abc", args[0]);
                    strings.add((String)args[0]);
                    return 1;
                });
                e.registerFunction("ns", "fn1", (Object... args) -> {
                    assertTrue(args[0] instanceof Double);
                    assertEquals(42.0, args[0]);
                    doubles.add((Double)args[0]);
                    return 2;
                });
                e.registerFunction("ns", "fn2", (Object... args) -> {
                    assertTrue(args[0] instanceof Boolean);
                    assertEquals(true, args[0]);
                    booleans.add((Boolean)args[0]);
                    return 3;
                });

                for (int i = 0; i < max; i++) {
                    e.execute(script);
                }

                assertEquals(max, strings.size());
                assertEquals(max, doubles.size());
                assertEquals(max, booleans.size());
            }
        });
    }


    @Test
    void shouldThrowExecutionExceptionWithLuaSyntaxError() {
        final String script = "print('Hello Lua!) -- unclosed string";
        final ExecutionException ex = assertThrows(ExecutionException.class, () -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.execute(script);
            }
        });
        assertEquals(LuaRetCode.LUA_ERRSYNTAX, ex.getCode());
    }

    @Test
    void shouldThrowFunctionException() {
        final String script = "ns:fn()";
        final ExecutionException ex = assertThrows(ExecutionException.class, () -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.execute(script);
            }
        });
        assertEquals(LuaRetCode.LUA_ERRRUN, ex.getCode());
        assertEquals("[string \"ns:fn()\"]:1: attempt to index a nil value (global 'ns')",
                ex.getLocalizedMessage());
    }

    @Test
    void shouldThrowExecutionExceptionWithNoScript() {
        final ExecutionException ex = assertThrows(ExecutionException.class, () -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.execute();
            }
        });
        assertEquals(LuaRetCode.LUA_ERRRUN, ex.getCode());
        assertEquals("no script to execute", ex.getLocalizedMessage());
    }

    @Test
    void foo() {
        final String script = "ns.fn()";

        final ExecutionException ex = assertThrows(ExecutionException.class, () -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.registerFunction("ns", "fn", (Object... args) -> {
                    assertEquals(0, args.length);
                    return 5;
                });
                e.execute(script);
            }
        });
        assertEquals(LuaRetCode.LUA_ERRRUN, ex.getCode());
        assertEquals("[string \"ns.fn()\"]:1: use ns:func() syntax", ex.getLocalizedMessage());
    }
}

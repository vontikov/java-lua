package com.githhub.vontikov.jl;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class LuaEngineTest {

    @Test
    void shouldExecuteScriptWithoutExternalCalls() {
        final String script =
                "print('Hello Lua!')";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.execute(script);
            }
        });
    }

    @Test
    void shouldCallExternalFunctionWithoutArguments() {
        final String script =
                "r = java.call('test')\n" +
                "print('call result: ', r)";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.setLogLevel("trace");
                final AtomicBoolean called = new AtomicBoolean();
                e.registerFunction("test", (Object... args) -> {
                    called.set(true);
                    return 0;
                });
                assertEquals(false, called.get());
                e.execute(script);
                assertEquals(true, called.get());
            }
        });
    }

    @Test
    void shouldCallExternalFunctionWithArguments() {
        final String script =
                "s = 'abc'\n" +
                "n = 42\n" +
                "b = true\n" +
                "r = java.call('test', s, n, b)\n" +
                "print('call result: ', r)";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.registerFunction("test", (Object... args) -> {
                    assertTrue(args[0] instanceof String);
                    assertEquals("abc", args[0]);

                    assertTrue(args[1] instanceof Double);
                    assertEquals(42.0, args[1]);

                    assertTrue(args[2] instanceof Boolean);
                    assertEquals(true, args[2]);
                    return 0;
                });
                e.execute(script);
            }
        });
    }

    @Test
    void shouldCallSeveralExternalFunctions() {
        final String script =
                "s = 'abc'\n" +
                "n = 42\n" +
                "b = true\n" +

                "r = java.call('fn0', s)\n" +
                "print('call result: ', r)\n" +

                "r = java.call('fn1', n)\n" +
                "print('call result: ', r)\n" +

                "r = java.call('fn2', b)\n" +
                "print('call result: ', r)\n";

        assertDoesNotThrow(() -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.registerFunction("fn0", (Object... args) -> {
                    assertTrue(args[0] instanceof String);
                    assertEquals("abc", args[0]);
                    return 0;
                });
                e.registerFunction("fn1", (Object... args) -> {
                    assertTrue(args[0] instanceof Double);
                    assertEquals(42.0, args[0]);
                    return 0;
                });
                e.registerFunction("fn2", (Object... args) -> {
                    assertTrue(args[0] instanceof Boolean);
                    assertEquals(true, args[0]);
                    return 0;
                });
                e.execute(script);
            }
        });
    }

    @Test
    void shouldThrowExecutionExceptionWithSyntaxError() {
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
        final String script = "r = java.call('does-not-exist')";
        final FunctionException ex = assertThrows(FunctionException.class, () -> {
            try (final LuaEngine e = new LuaEngine()) {
                e.execute(script);
            }
        });
        assertEquals("function not found: does-not-exist", ex.getLocalizedMessage());
    }
}

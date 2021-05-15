# java_lua

Provides the ability to execute [Lua 5.4.3](https://www.lua.org/about.html)
scripts from Java.

Lua scripts can call registered in a namespace Java functions:

```
    final String script =
            "r = java:fn(42, 'xyz')\n" +
            "print('call result: ', r) -- prints 100\n" +

            "r = ns:func('abc', 42, true, false)\n" +
            "print('call result: ', r) -- prints 200\n";

      try (final LuaEngine e = new LuaEngine()) {

          // register function in the default namespace 'java'
          e.registerFunction("fn", (Object... args) -> {
              // do something
              ...

              // return call result to the script
              return 100;
          });

          // register external function in the custom namespace 'ns'
          e.registerFunction("ns", "func", (Object... args) -> {
              // do something
              ...

              // return call result to the script
              return 200;
          });

          // load and execute
          e.execute(script);

          // ... or load
          e.load(script);
          // and execute
          e.execute();

          ...

          // execute again
          e.execute();
      }
```
See Java unit tests for the details

## Build

```
gradle build
```

## Distribution

```
gradle assembleDist
```

## Run

The `java.library.path` system property should contain path to the folder with
`libjl.so`.

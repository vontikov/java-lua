# java_lua

Provides the ability to execute [Lua 5.4.3](https://www.lua.org/about.html)
scripts from Java.

The Lua scripts can call previously registered Java functions (see unit tests).

## Build

```
gradle build
```

## Distribution

```
gradle assembleDist
```

## Run

The system property `java.library.path` should contain path to the folder with
`libjl.so`.

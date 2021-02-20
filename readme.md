# Kaleidoscope

![GitHub](https://img.shields.io/github/license/AoraMD/kaleidoscope?style=flat-square&color=9fa8da)

Kaleidoscope is an Android dynamic hooking framework for intercepting function calls in the application process.

This library is created for learning Android hook, so it is a demo-level library and is not stable enough to run in any production environment.

## Support

Version : Android 5 ~ Android 11

Architecture: arm64

## Usage

Kaleidoscope needs to be initialized before using.

``` kotlin
class MainApplication : Application() {

    override fun attachBaseContext(base: Context) {
        super.attachBaseContext(base)
        base.initializeKaleidoscope(LogLevel.DEBUG)
    }
}
```

Then you can listen or replace method calls.

``` kotlin
val scope = method.listen()
            .before {
                System.currentTimeMillis()
            }
            .after { thiz, store ->
                val time = System.currentTimeMillis() - store as Long
                Log.i("Kaleidoscope Sample", "The method of object $thiz is called, which takes $time milliseconds.")
            }
            .commit()

// Restore method settings.
scope.restore()
```

## Thanks

[tiann - Epic](https://github.com/tiann/epic)

[canyie - Pine](https://github.com/canyie/pine)

[ganyao114 - SandHook](https://github.com/ganyao114/SandHook)

## License

```
MIT License

Copyright (c) 2020 M.D.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```


package moe.aoramd.kaleidoscope.test

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import moe.aoramd.kaleidoscope.listen

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val scope = Test::class.java.getDeclaredMethod(
            "argumentCheck",
            Byte::class.java,
            Short::class.java,
            Int::class.java,
            Long::class.java,
            Float::class.java,
            Double::class.java,
            Boolean::class.java,
            Byte::class.java,
            Char::class.java,
            Short::class.java,
            Int::class.java,
            Long::class.java,
            Float::class.java,
            Double::class.java,
            Any::class.java,
            Long::class.java,
            Any::class.java,
            Long::class.java
        ).listen()
            .before { _, _ ->
                System.currentTimeMillis()
            }
            .after { thiz, _, state ->
                val time = System.currentTimeMillis() - state as Long
                Log.i(
                    "Kaleidoscope Sample",
                    "The method of object $thiz is called, which takes $time milliseconds."
                )
            }
            .commit()

        Test().apply {
            val result = callArgumentCheck(this@MainActivity, 102030405060708090)
            Log.i("Kaleidoscope Sample", "callArgumentCheck() return value is $result")
            scope.restore()
            callArgumentCheck(this@MainActivity, 102030405060708090)
        }

        val staticScope = Test::class.java.getDeclaredMethod(
            "argumentCheckStatic",
            Byte::class.java,
            Short::class.java,
            Int::class.java,
            Long::class.java,
            Float::class.java,
            Double::class.java,
            Boolean::class.java,
            Byte::class.java,
            Char::class.java,
            Short::class.java,
            Int::class.java,
            Long::class.java,
            Float::class.java,
            Double::class.java,
            Any::class.java,
            Long::class.java,
            Any::class.java,
            Long::class.java
        ).listen()
            .before { _, _ ->
                System.currentTimeMillis()
            }
            .after { thiz, _, store ->
                val time = System.currentTimeMillis() - store as Long
                Log.i(
                    "Kaleidoscope Sample",
                    "The method of object $thiz is called, which takes $time milliseconds."
                )
            }
            .commit()

        Test.callArgumentCheckStatic(this@MainActivity, 0x1020304050607080)
        staticScope.restore()
        Test.callArgumentCheckStatic(this@MainActivity, 0x1020304050607080)
    }
}
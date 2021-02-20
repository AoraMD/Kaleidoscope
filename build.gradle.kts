buildscript {
    repositories {
        google()
        jcenter()
    }
    dependencies {
        classpath(Deps.Project.androidGradlePlugin)
        classpath(Deps.Kotlin.gradlePlugin)
    }
}

allprojects {
    repositories {
        google()
        jcenter()
    }
}

task<Delete>("clean") {
    delete(rootProject.buildDir)
}
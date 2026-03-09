#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <string>
#include <algorithm>

// Константы из шейдера
#define NUM_NOISE_OCTAVES 10
#define PLANET_SIZE 0.75

// Опционально: раскомментировать для sharpen фильтра
//#define SHARPEN

// Простое сглаживание краев
#define SMOOTH

// Режимы генерации поверхности
enum SurfaceMode {
    MODE_DEFAULT = 0,      // Стандартный режим (из оригинального шейдера)
    MODE_CONTINENTS,       // Четкие континенты
    MODE_STRIPES,          // Полосатая планета
    MODE_SPOTS,            // Пятнистая планета
    MODE_VEINS,            // С венами/трещинами
    MODE_CELLULAR,         // Клеточная структура
    MODE_WHIRLPOOL,        // Вихревая структура
    MODE_RIDGED,           // Гребнистая поверхность
    MODE_CRATERS,          // Кратеры
    MODE_CLOUDS,           // Облачная структура
    MODE_RANDOM            // Случайный режим при каждой генерации
};

// Структура для 3D вектора
struct vec3 {
    float x, y, z;

    vec3() : x(0), y(0), z(0) {}
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    vec3 operator+(const vec3& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
    vec3 operator-(const vec3& v) const { return vec3(x - v.x, y - v.y, z - v.z); }
    vec3 operator*(float s) const { return vec3(x * s, y * s, z * s); }
    vec3 operator/(float s) const { return vec3(x / s, y / s, z / s); }

    vec3& operator+=(const vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
};

// Скалярное произведение
float dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Нормализация вектора
vec3 normalize(const vec3& v) {
    float len = std::sqrt(dot(v, v));
    if (len > 0) return vec3(v.x / len, v.y / len, v.z / len);
    return v;
}

// Смешивание (линейная интерполяция)
float mix(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}

// Векторное смешивание
vec3 mix(const vec3& a, const vec3& b, float t) {
    return vec3(mix(a.x, b.x, t), mix(a.y, b.y, t), mix(a.z, b.z, t));
}

// Функция clamp
float clamp(float x, float minVal, float maxVal) {
    if (x < minVal) return minVal;
    if (x > maxVal) return maxVal;
    return x;
}

// Синусоидальная интерполяция
float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// Структура для матрицы 3x3
struct mat3 {
    float m[3][3];

    mat3() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    mat3(float a00, float a01, float a02,
         float a10, float a11, float a12,
         float a20, float a21, float a22) {
        m[0][0] = a00; m[0][1] = a01; m[0][2] = a02;
        m[1][0] = a10; m[1][1] = a11; m[1][2] = a12;
        m[2][0] = a20; m[2][1] = a21; m[2][2] = a22;
    }

    vec3 multiply(const vec3& v) const {
        return vec3(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
        );
    }
};

// Векторный clamp
vec3 clamp(const vec3& v, float minVal, float maxVal) {
    return vec3(
        clamp(v.x, minVal, maxVal),
        clamp(v.y, minVal, maxVal),
        clamp(v.z, minVal, maxVal)
    );
}

// Функция для случайного выбора режима
SurfaceMode getRandomMode(std::mt19937& rng) {
    std::uniform_int_distribution<int> modeDist(0, 9); // 10 режимов (MODE_RANDOM除外)
    return static_cast<SurfaceMode>(modeDist(rng));
}

class PlanetGenerator {
private:
    // ГПСЧ (Генератор псевдослучайных чисел)
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    // Параметры для цветовой палитры
    float iTime;
    float iTimeOffset;

    // Параметры поверхности
    SurfaceMode surfaceMode;
    float noiseScale;           // Масштаб шума
    float warpAmount;           // Степень искривления
    float ridgeAmount;          // Степень гребнистости
    float cellularSize;         // Размер клеточной структуры
    float stripeFrequency;      // Частота полос
    float spotSize;             // Размер пятен
    float craterAmount;         // Количество кратеров

    // Размеры изображения
    int width, height;

    // Структура для хранения пикселя BMP (24 бита)
    #pragma pack(push, 1)
    struct BMPPixel {
        uint8_t b, g, r; // BMP хранит в порядке BGR
    };
    #pragma pack(pop)

    // Структура заголовка BMP файла
    #pragma pack(push, 1)
    struct BMPHeader {
        uint16_t bfType = 0x4D42;        // "BM"
        uint32_t bfSize;                  // Размер файла
        uint16_t bfReserved1 = 0;
        uint16_t bfReserved2 = 0;
        uint32_t bfOffBits = 54;          // Смещение до данных пикселей

        // Информационный заголовок
        uint32_t biSize = 40;              // Размер этого заголовка
        int32_t  biWidth;                   // Ширина
        int32_t  biHeight;                   // Высота (положительное значение = снизу вверх)
        uint16_t biPlanes = 1;
        uint16_t biBitCount = 24;           // 24 бита на пиксель
        uint32_t biCompression = 0;
        uint32_t biSizeImage;                // Размер изображения в байтах
        int32_t  biXPelsPerMeter = 2835;     // 72 DPI
        int32_t  biYPelsPerMeter = 2835;
        uint32_t biClrUsed = 0;
        uint32_t biClrImportant = 0;
    };
    #pragma pack(pop)

    // Вспомогательные функции
    float square(float x) { return x * x; }
    float infIfNegative(float x) { return (x >= 0.0) ? x : 9999999.9f; }

    float max3(const vec3& v) {
        return std::max(std::max(v.x, v.y), v.z);
    }

    float fract(float x) {
        return x - std::floor(x);
    }

    // Хеш-функция (из шейдера)
    float hash(float p) {
        p = fract(p * 0.011f);
        p *= p + 7.5f;
        p *= p + p;
        return fract(p);
    }

    // 3D шум (из шейдера)
    float noise(const vec3& x) {
        const vec3 step = vec3(110, 241, 171);
        vec3 i = vec3(std::floor(x.x), std::floor(x.y), std::floor(x.z));
        vec3 f = vec3(fract(x.x), fract(x.y), fract(x.z));

        float n = dot(i, step);

        vec3 u = vec3(f.x * f.x * (3.0f - 2.0f * f.x),
                      f.y * f.y * (3.0f - 2.0f * f.y),
                      f.z * f.z * (3.0f - 2.0f * f.z));

        float h1 = hash(n + dot(step, vec3(0, 0, 0)));
        float h2 = hash(n + dot(step, vec3(1, 0, 0)));
        float h3 = hash(n + dot(step, vec3(0, 1, 0)));
        float h4 = hash(n + dot(step, vec3(1, 1, 0)));
        float h5 = hash(n + dot(step, vec3(0, 0, 1)));
        float h6 = hash(n + dot(step, vec3(1, 0, 1)));
        float h7 = hash(n + dot(step, vec3(0, 1, 1)));
        float h8 = hash(n + dot(step, vec3(1, 1, 1)));

        float mix1 = mix(mix(h1, h2, u.x), mix(h3, h4, u.x), u.y);
        float mix2 = mix(mix(h5, h6, u.x), mix(h7, h8, u.x), u.y);

        return mix(mix1, mix2, u.z);
    }

    // Абсолютное значение шума для гребнистого эффекта
    float noiseAbs(const vec3& x) {
        return std::abs(noise(x) * 2.0f - 1.0f);
    }

    // Клеточный шум (для режима CELLULAR)
    float cellularNoise(const vec3& x) {
        vec3 p = vec3(x.x * cellularSize, x.y * cellularSize, x.z * cellularSize);
        vec3 i = vec3(std::floor(p.x), std::floor(p.y), std::floor(p.z));
        vec3 f = vec3(fract(p.x), fract(p.y), fract(p.z));

        float minDist = 1.0f;

        for (int k = -1; k <= 1; k++) {
            for (int j = -1; j <= 1; j++) {
                for (int l = -1; l <= 1; l++) {
                    vec3 cell = vec3(i.x + l, i.y + j, i.z + k);
                    vec3 cellOffset = vec3(
                        hash(dot(cell, vec3(1, 57, 113))),
                        hash(dot(cell, vec3(57, 113, 1))),
                        hash(dot(cell, vec3(113, 1, 57)))
                    );

                    vec3 diff = vec3(l - f.x + cellOffset.x,
                                     j - f.y + cellOffset.y,
                                     k - f.z + cellOffset.z);
                    float dist = dot(diff, diff);

                    if (dist < minDist) {
                        minDist = dist;
                    }
                }
            }
        }

        return clamp(1.0f - std::sqrt(minDist), 0.0f, 1.0f);
    }

    // Функция для создания пятен
    float spotNoise(const vec3& x) {
        float n = noise(vec3(x.x * spotSize, x.y * spotSize, x.z * spotSize));
        return smoothstep(0.3f, 0.7f, n);
    }

    // Функция для создания полос
    float stripeNoise(const vec3& x) {
        float stripe = std::sin(x.y * stripeFrequency * 2.0f * 3.14159f) *
                       std::cos(x.x * stripeFrequency * 0.5f * 3.14159f);
        stripe = stripe * 0.5f + 0.5f;

        // Добавляем шум для разнообразия
        float n = noise(vec3(x.x * 2.0f, x.y * 2.0f, x.z * 2.0f));
        return mix(stripe, n, 0.3f);
    }

    // Функция для создания кратеров
    float craterNoise(const vec3& x) {
        float n = noise(vec3(x.x * 3.0f, x.y * 3.0f, x.z * 3.0f));
        float craters = 0.0f;

        for (int i = 0; i < 3; i++) {
            vec3 offset = vec3(
                hash(i * 10) * 10.0f,
                hash(i * 10 + 1) * 10.0f,
                hash(i * 10 + 2) * 10.0f
            );

            vec3 p = vec3(x.x * 2.0f + offset.x,
                          x.y * 2.0f + offset.y,
                          x.z * 2.0f + offset.z);
            float dist = std::sqrt(dot(p, p)) * 0.5f;
            craters += std::max(0.0f, 1.0f - dist * 2.0f) * (1.0f - n);
        }

        return clamp(craters * craterAmount, 0.0f, 1.0f);
    }

    // FBM (Fractional Brownian Motion) с вариациями
    float fbm(vec3 x, float scale = 1.0f) {
        float v = 0.0f;
        float a = 0.5f;
        vec3 shift = vec3(100, 100, 100);

        x = vec3(x.x * scale, x.y * scale, x.z * scale);

        for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
            v += a * noise(x);
            x = vec3(x.x * 2.0f + shift.x,
                     x.y * 2.0f + shift.y,
                     x.z * 2.0f + shift.z);
            a *= 0.5f;
        }
        return v;
    }

    // Модифицированный FBM с искривлением (domain warping)
    float fbmWarped(vec3 x) {
        vec3 q = vec3(
            fbm(x),
            fbm(vec3(x.x + 1.0f, x.y, x.z)),
            fbm(vec3(x.x, x.y + 1.0f, x.z))
        );

        vec3 r = vec3(
            fbm(vec3(x.x + q.x * warpAmount, x.y + q.y * warpAmount, x.z + q.z * warpAmount)),
            fbm(vec3(x.x + 1.0f + q.x * warpAmount * 0.5f, x.y + q.y * warpAmount * 0.5f, x.z + q.z * warpAmount * 0.5f)),
            fbm(vec3(x.x + q.x * warpAmount * 0.5f, x.y + 1.0f + q.y * warpAmount * 0.5f, x.z + q.z * warpAmount * 0.5f))
        );

        return fbm(vec3(x.x + r.x * warpAmount * 0.25f,
                        x.y + r.y * warpAmount * 0.25f,
                        x.z + r.z * warpAmount * 0.25f));
    }

    // Пересечение луча со сферой
    float intersectSphere(const vec3& C, float r, const vec3& P, const vec3& w) {
        vec3 v = vec3(P.x - C.x, P.y - C.y, P.z - C.z);
        float b = -dot(w, v);
        float c = dot(v, v) - square(r);
        float d = (square(b) - c);

        if (d < 0.0f) { return 9999999.9f; }

        float dsqrt = std::sqrt(d);
        float t1 = b - dsqrt;
        float t2 = b + dsqrt;

        return std::min(infIfNegative(t1), infIfNegative(t2));
    }

    // Косинусная палитра (из шейдера)
    vec3 palette(float t, const vec3& a, const vec3& b, const vec3& c, const vec3& d) {
        return vec3(a.x + b.x * std::cos(6.28318f * (c.x * t + d.x)),
                    a.y + b.y * std::cos(6.28318f * (c.y * t + d.y)),
                    a.z + b.z * std::cos(6.28318f * (c.z * t + d.z)));
    }

    // Получение значения высоты/структуры в зависимости от режима
    float getSurfaceValue(const vec3& X) {
        float val = 0.0f;

        switch (surfaceMode) {
            case MODE_CONTINENTS:
                // Четкие континенты с резкими границами
                val = fbm(X, noiseScale);
                val = smoothstep(0.3f, 0.7f, val);
                break;

            case MODE_STRIPES:
                // Полосатая структура
                val = stripeNoise(X);
                break;

            case MODE_SPOTS:
                // Пятнистая структура
                val = spotNoise(X);
                break;

            case MODE_VEINS:
                // Венозная/трещиноватая структура
                val = fbm(X, noiseScale * 2.0f);
                val = std::pow(val, 2.0f); // Усиливаем контраст
                break;

            case MODE_CELLULAR:
                // Клеточная структура
                val = cellularNoise(X);
                break;

            case MODE_WHIRLPOOL:
                // Вихревая структура
                {
                    float angle = std::atan2(X.y, X.x);
                    float radius = std::sqrt(X.x * X.x + X.y * X.y);
                    float spiral = std::sin(angle * 5.0f + radius * 10.0f) * 0.5f + 0.5f;
                    float n = fbm(X, noiseScale);
                    val = mix(spiral, n, 0.5f);
                }
                break;

            case MODE_RIDGED:
                // Гребнистая структура
                {
                    vec3 q = vec3(X.x * noiseScale, X.y * noiseScale, X.z * noiseScale);
                    val = 0.0f;
                    float a = 0.5f;
                    for (int i = 0; i < 5; i++) {
                        val += a * (1.0f - std::abs(noise(q) * 2.0f - 1.0f));
                        q = vec3(q.x * 2.0f, q.y * 2.0f, q.z * 2.0f);
                        a *= 0.5f;
                    }
                }
                break;

            case MODE_CRATERS:
                // Кратеры
                val = craterNoise(X);
                break;

            case MODE_CLOUDS:
                // Облачная структура
                val = fbmWarped(X);
                val = std::max(0.0f, val - 0.3f) * 1.5f;
                break;

            case MODE_DEFAULT:
            default:
                // Стандартный режим с искривлением
                {
                    vec3 q = vec3(
                        fbm(vec3(X.x + 0.025f * iTime, X.y, X.z)),
                        fbm(X),
                        fbm(X)
                    );

                    vec3 r = vec3(
                        fbm(vec3(X.x + q.x, X.y + q.y, X.z + q.z) + vec3(0.01f * iTime, 0, 0)),
                        fbm(vec3(X.x + q.x, X.y + q.y, X.z + q.z)),
                        fbm(vec3(X.x + q.x, X.y + q.y, X.z + q.z))
                    );

                    val = fbm(vec3(X.x + r.x * 5.0f + iTime * 0.005f,
                                   X.y + r.y * 5.0f,
                                   X.z + r.z * 5.0f));
                }
                break;
        }

        return clamp(val, 0.0f, 1.0f);
    }

    // Получение цвета для координаты
    vec3 getColorForCoord(float x, float y) {
        vec3 color = vec3(0, 0, 0);

        // Вращение планеты
        float theta = iTime * 0.15f;
        mat3 rot = mat3(
            cos(theta), 0, sin(theta),
            0, 1, 0,
            -sin(theta), 0, cos(theta)
        );

        // Настройки камеры
        const float verticalFieldOfView = 25.0f * 3.1415926535f / 180.0f;

        // Позиция камеры и направление луча
        vec3 P = vec3(0.0f, 0.0f, 5.0f);

        float screenX = (x - width * 0.5f);
        float screenY = (y - height * 0.5f);
        float screenZ = (height) / (-2.0f * std::tan(verticalFieldOfView / 2.0f));

        vec3 w = normalize(vec3(screenX, screenY, screenZ));

        // Поиск пересечения со сферой
        float t = intersectSphere(vec3(0, 0, 0), PLANET_SIZE, P, w);

        float v = 0.0f;

        if (t < 9999999.0f) {
            // Точка пересечения
            vec3 X = vec3(P.x + w.x * t, P.y + w.y * t, P.z + w.z * t);

            // Применяем вращение
            X = rot.multiply(X);

            // Получаем значение поверхности в зависимости от режима
            v = getSurfaceValue(X);
        } else {
            return vec3(0, 0, 0);
        }

        // Параметры палитры с вариациями в зависимости от режима
        vec3 a, b, c, d;

        // Разные цветовые схемы для разных режимов
        switch (surfaceMode) {
            case MODE_CONTINENTS:
                a = vec3(0.2f, 0.3f, 0.4f);
                b = vec3(0.4f, 0.5f, 0.3f);
                c = vec3(0.5f, 0.4f, 0.6f);
                d = vec3(0.1f, 0.2f, 0.3f);
                break;

            case MODE_STRIPES:
                a = vec3(0.5f, 0.2f, 0.2f);
                b = vec3(0.3f, 0.5f, 0.5f);
                c = vec3(0.7f, 0.3f, 0.4f);
                d = vec3(0.1f, 0.4f, 0.2f);
                break;

            case MODE_SPOTS:
                a = vec3(0.3f, 0.5f, 0.2f);
                b = vec3(0.5f, 0.2f, 0.5f);
                c = vec3(0.4f, 0.6f, 0.3f);
                d = vec3(0.2f, 0.1f, 0.5f);
                break;

            case MODE_VEINS:
                a = vec3(0.1f, 0.1f, 0.1f);
                b = vec3(0.5f, 0.3f, 0.2f);
                c = vec3(0.6f, 0.4f, 0.3f);
                d = vec3(0.2f, 0.1f, 0.0f);
                break;

            case MODE_CELLULAR:
                a = vec3(0.2f, 0.5f, 0.2f);
                b = vec3(0.3f, 0.2f, 0.5f);
                c = vec3(0.4f, 0.3f, 0.6f);
                d = vec3(0.1f, 0.2f, 0.3f);
                break;

            case MODE_WHIRLPOOL:
                a = vec3(0.3f, 0.2f, 0.5f);
                b = vec3(0.5f, 0.3f, 0.2f);
                c = vec3(0.4f, 0.6f, 0.3f);
                d = vec3(0.2f, 0.1f, 0.4f);
                break;

            case MODE_RIDGED:
                a = vec3(0.3f, 0.2f, 0.1f);
                b = vec3(0.4f, 0.3f, 0.2f);
                c = vec3(0.5f, 0.4f, 0.3f);
                d = vec3(0.1f, 0.1f, 0.1f);
                break;

            case MODE_CRATERS:
                a = vec3(0.4f, 0.3f, 0.2f);
                b = vec3(0.3f, 0.3f, 0.3f);
                c = vec3(0.2f, 0.2f, 0.2f);
                d = vec3(0.1f, 0.1f, 0.1f);
                break;

            case MODE_CLOUDS:
                a = vec3(0.1f, 0.2f, 0.3f);
                b = vec3(0.3f, 0.4f, 0.5f);
                c = vec3(0.5f, 0.6f, 0.7f);
                d = vec3(0.0f, 0.1f, 0.2f);
                break;

            default:
                a = vec3(0.5f, 0.25f + (std::sin(iTime * 0.1f) - 0.25f + 0.25f), 0.5f);
                b = vec3(0.5f, 0.5f, 0.5f);
                c = vec3(1.0f, 1.0f, 1.0f);
                d = vec3(0.00f, 0.33f, 0.67f);
                break;
        }

        // Масштабируем цвета
        a.x *= 0.5f; a.y *= 0.5f; a.z *= 0.5f;
        b.x *= 0.5f; b.y *= 0.5f; b.z *= 0.5f;
        c.x *= 0.5f; c.y *= 0.5f; c.z *= 0.5f;
        d.x *= 0.5f; d.y *= 0.5f; d.z *= 0.5f;

        // Генерируем цвета с использованием iTimeOffset для разнообразия
        vec3 col_top = palette(iTime * 0.11f + iTimeOffset, a, b, c, d);
        col_top.x *= 0.5f; col_top.y *= 0.5f; col_top.z *= 0.5f;

        vec3 col_bot = palette(iTime * 0.115f + iTimeOffset, a, b, c, d);
        col_bot.x *= 0.5f; col_bot.y *= 0.5f; col_bot.z *= 0.5f;

        vec3 col_mid1 = palette(iTime * 0.1135f + iTimeOffset, a, b, c, d);
        col_mid1.x *= 0.33f; col_mid1.y *= 0.33f; col_mid1.z *= 0.33f;

        vec3 col_mid2 = palette(iTime * -0.1135f + iTimeOffset, d, c, b, a);
        col_mid2.x *= 0.33f; col_mid2.y *= 0.33f; col_mid2.z *= 0.33f;

        vec3 col_mid3 = palette(iTime * -0.114f + iTimeOffset, c, d, c, b);
        col_mid3.x *= 0.33f; col_mid3.y *= 0.33f; col_mid3.z *= 0.33f;

        // Смешивание средних цветов
        vec3 col_mid = mix(col_mid1, col_mid2, v);
        col_mid = mix(col_mid, col_mid3, v * 0.5f);

        // Финальное смешивание на основе высоты с дополнительными вариациями
        float pos = v * 2.0f - 1.0f;

        // Добавляем небольшую вариацию в зависимости от режима
        float variation = 0.0f;
        if (surfaceMode == MODE_RIDGED || surfaceMode == MODE_VEINS) {
            variation = std::sin(v * 10.0f) * 0.1f;
        }

        float clampPos = clamp(pos + variation, 0.0f, 1.0f);
        float clampNegPos = clamp(-pos + variation, 0.0f, 1.0f);

        color = mix(col_mid, col_top, clampPos);
        color = mix(color, col_bot, clampNegPos);

        // Нормализация цвета
        float maxComp = max3(color);
        if (maxComp > 0.0f) {
            color.x /= maxComp;
            color.y /= maxComp;
            color.z /= maxComp;
        }

        // Коррекция яркости с вариациями для разных режимов
        float brightness = v;
        if (surfaceMode == MODE_RIDGED) {
            brightness = v * v; // Более темные впадины
        } else if (surfaceMode == MODE_CLOUDS) {
            brightness = 0.7f + v * 0.3f; // Более равномерная яркость
        } else {
            brightness = clamp((0.4f * std::pow(v, 3.0f) + std::pow(v, 2.0f) + 0.5f * v), 0.0f, 1.0f);
        }

        brightness = brightness * 0.9f + 0.1f;
        color.x *= brightness;
        color.y *= brightness;
        color.z *= brightness;

        // Диффузное освещение
        vec3 lightDir = vec3(1.0f, std::sqrt(0.5f), 1.0f);
        lightDir = normalize(lightDir);

        vec3 surfaceNormal = normalize(vec3(P.x + w.x * t, P.y + w.y * t, P.z + w.z * t));
        float diffuse = std::max(0.0f, dot(surfaceNormal, lightDir));
        float ambient = 0.1f;

        float lighting = clamp(diffuse + ambient, 0.0f, 1.0f);
        color.x *= lighting;
        color.y *= lighting;
        color.z *= lighting;

#ifdef SMOOTH
        // Сглаживание краев
        float smoothFactor = (P.z + w.z * t) * 2.0f;
        color.x *= smoothFactor;
        color.y *= smoothFactor;
        color.z *= smoothFactor;
#endif

        return color;
    }

    // Инициализация случайных параметров поверхности
    void randomizeSurfaceParams() {
        // Случайный выбор режима (исключая MODE_RANDOM)
        std::uniform_int_distribution<int> modeDist(0, 9);
        surfaceMode = static_cast<SurfaceMode>(modeDist(rng));

        // Случайные параметры
        noiseScale = dist(rng) * 2.0f + 0.5f;           // 0.5 - 2.5
        warpAmount = dist(rng) * 2.0f;                  // 0 - 2
        ridgeAmount = dist(rng) * 0.5f;                  // 0 - 0.5
        cellularSize = dist(rng) * 3.0f + 1.0f;         // 1 - 4
        stripeFrequency = dist(rng) * 5.0f + 2.0f;       // 2 - 7
        spotSize = dist(rng) * 3.0f + 1.0f;              // 1 - 4
        craterAmount = dist(rng) * 0.8f + 0.2f;          // 0.2 - 1.0
    }

public:
    PlanetGenerator(int w, int h) : width(w), height(h), dist(0.0f, 1.0f) {
        // Инициализация ГПСЧ случайным зерном
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));

        // Начальные значения времени
        iTime = dist(rng) * 10.0f;
        iTimeOffset = dist(rng) * 100.0f;

        // Случайные параметры поверхности
        randomizeSurfaceParams();
    }

    // Установка конкретного режима поверхности
    void setSurfaceMode(SurfaceMode mode) {
        if (mode == MODE_RANDOM) {
            randomizeSurfaceParams();
        } else {
            surfaceMode = mode;
        }
    }

    // Установка конкретного зерна для воспроизводимости
    void setSeed(unsigned int seed) {
        rng.seed(seed);
        iTime = dist(rng) * 10.0f;
        iTimeOffset = dist(rng) * 100.0f;
        randomizeSurfaceParams(); // Случайные параметры на основе нового seed
    }

    // Получение названия текущего режима
    std::string getModeName() const {
        switch (surfaceMode) {
            case MODE_DEFAULT: return "Default";
            case MODE_CONTINENTS: return "Continents";
            case MODE_STRIPES: return "Stripes";
            case MODE_SPOTS: return "Spots";
            case MODE_VEINS: return "Veins";
            case MODE_CELLULAR: return "Cellular";
            case MODE_WHIRLPOOL: return "Whirlpool";
            case MODE_RIDGED: return "Ridged";
            case MODE_CRATERS: return "Craters";
            case MODE_CLOUDS: return "Clouds";
            default: return "Unknown";
        }
    }

    // Генерация и сохранение планеты
    bool generateAndSave(const std::string& filename) {
        std::vector<BMPPixel> pixels(width * height);

        std::cout << "Генерация планеты в режиме: " << getModeName() << "\n";
        std::cout << "Параметры: scale=" << noiseScale
                  << ", warp=" << warpAmount
                  << ", freq=" << stripeFrequency << "\n";

        // Генерация пикселей
        #pragma omp parallel for collapse(2)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vec3 color;

#ifdef SHARPEN
                // Sharpen фильтр
                vec3 c0 = getColorForCoord(x, y);
                vec3 c1 = getColorForCoord(x + 1, y);
                vec3 c2 = getColorForCoord(x, y + 1);
                vec3 c3 = getColorForCoord(x - 1, y);
                vec3 c4 = getColorForCoord(x, y - 1);

                color.x = c0.x * 3.0f - c1.x * 0.5f - c2.x * 0.5f - c3.x * 0.5f - c4.x * 0.5f;
                color.y = c0.y * 3.0f - c1.y * 0.5f - c2.y * 0.5f - c3.y * 0.5f - c4.y * 0.5f;
                color.z = c0.z * 3.0f - c1.z * 0.5f - c2.z * 0.5f - c3.z * 0.5f - c4.z * 0.5f;

                color = clamp(color, 0.0f, 1.0f);
#else
                color = getColorForCoord(x, y);
#endif

                // Конвертация float [0,1] в uint8_t [0,255]
                BMPPixel& pixel = pixels[y * width + x];
                pixel.r = static_cast<uint8_t>(std::min(255.0f, std::max(0.0f, color.x * 255.0f)));
                pixel.g = static_cast<uint8_t>(std::min(255.0f, std::max(0.0f, color.y * 255.0f)));
                pixel.b = static_cast<uint8_t>(std::min(255.0f, std::max(0.0f, color.z * 255.0f)));
            }
        }

        std::cout << "Сохранение в " << filename << "...\n";

        // Сохранение в BMP
        return saveBMP(filename, pixels);
    }

    bool saveBMP(const std::string& filename, const std::vector<BMPPixel>& pixels) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Ошибка: не удалось создать файл " << filename << std::endl;
            return false;
        }

        // Выравнивание строк до 4 байт
        int rowSize = (width * 3 + 3) & ~3;
        int imageSize = rowSize * height;

        BMPHeader header;
        header.biWidth = width;
        header.biHeight = height;
        header.biSizeImage = imageSize;
        header.bfSize = 54 + imageSize;

        // Запись заголовка
        file.write(reinterpret_cast<char*>(&header), sizeof(header));

        // Запись пикселей (снизу вверх для BMP)
        std::vector<uint8_t> row(rowSize, 0);
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                const BMPPixel& pixel = pixels[y * width + x];
                row[x * 3 + 0] = pixel.b;
                row[x * 3 + 1] = pixel.g;
                row[x * 3 + 2] = pixel.r;
            }
            file.write(reinterpret_cast<char*>(row.data()), rowSize);
        }

        file.close();
        std::cout << "Готово! Файл сохранен: " << filename << std::endl;
        return true;
    }

    // Генерация нескольких планет с разными параметрами
    void generateMultiple(int count, const std::string& baseName) {
        for (int i = 0; i < count; ++i) {
            // Новое случайное зерно для каждой планеты
            setSeed(rng());

            std::string filename = baseName + "_" + getModeName() + "_" + std::to_string(i + 1) + ".bmp";
            generateAndSave(filename);
        }
    }
};

// Вспомогательная функция для разбора аргументов командной строки
void printHelp() {
    std::cout << "Использование: planet_generator [опции]\n"
              << "Опции:\n"
              << "  -w, --width <ширина>     Ширина изображения (по умолчанию 800)\n"
              << "  -h, --height <высота>    Высота изображения (по умолчанию 600)\n"
              << "  -o, --output <файл>       Имя выходного файла (по умолчанию planet.bmp)\n"
              << "  -c, --count <количество>  Количество генерируемых планет\n"
              << "  -s, --seed <зерно>        Зерно ГПСЧ для воспроизводимости\n"
              << "  -m, --mode <режим>        Режим поверхности (0-10, где 10=случайный)\n"
              << "                             0: Default, 1: Continents, 2: Stripes,\n"
              << "                             3: Spots, 4: Veins, 5: Cellular,\n"
              << "                             6: Whirlpool, 7: Ridged, 8: Craters,\n"
              << "                             9: Clouds, 10: Random\n"
              << "  --help                     Показать эту справку\n";
}

int main(int argc, char* argv[]) {
    int width = 800;
    int height = 600;
    std::string outputFile = "planet.bmp";
    int count = 1;
    unsigned int seed = 0;
    int mode = 10; // По умолчанию случайный режим
    bool useRandomSeed = true;
    bool modeSet = false;
    
    std::system("chcp 65001");

    // Разбор аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-w" || arg == "--width") {
            if (i + 1 < argc) width = std::stoi(argv[++i]);
        } else if (arg == "-h" || arg == "--height") {
            if (i + 1 < argc) height = std::stoi(argv[++i]);
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) outputFile = argv[++i];
        } else if (arg == "-c" || arg == "--count") {
            if (i + 1 < argc) count = std::stoi(argv[++i]);
        } else if (arg == "-s" || arg == "--seed") {
            if (i + 1 < argc) {
                seed = static_cast<unsigned int>(std::stoul(argv[++i]));
                useRandomSeed = false;
            }
        } else if (arg == "-m" || arg == "--mode") {
            if (i + 1 < argc) {
                mode = std::stoi(argv[++i]);
                modeSet = true;
            }
        } else if (arg == "--help") {
            printHelp();
            return 0;
        }
    }

    std::cout << "Генератор планет с вариативной поверхностью\n";
    std::cout << "Размер: " << width << "x" << height << "\n";

    PlanetGenerator generator(width, height);

    if (!useRandomSeed) {
        generator.setSeed(seed);
        std::cout << "Используется зерно: " << seed << "\n";
    } else {
        std::cout << "Используется случайное зерно\n";
    }

    if (modeSet) {
        if (mode >= 0 && mode <= 10) {
            generator.setSurfaceMode(static_cast<SurfaceMode>(mode));
        } else {
            std::cout << "Неверный режим. Используется случайный режим.\n";
        }
    }

    if (count > 1) {
        // Убираем расширение .bmp из имени файла для множественной генерации
        size_t dotPos = outputFile.find_last_of('.');
        std::string baseName = (dotPos != std::string::npos) ?
                               outputFile.substr(0, dotPos) : outputFile;
        generator.generateMultiple(count, baseName);
    } else {
        generator.generateAndSave(outputFile);
    }

    return 0;
}

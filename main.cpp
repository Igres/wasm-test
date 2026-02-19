#include <vector>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

struct Unit {
    int x;
    int y;
};

static std::vector<Unit> g_units;

/*
    Возвращаем указатель напрямую (без embind)
*/
extern "C" {

EMSCRIPTEN_KEEPALIVE
int* get_units_buffer_ptr() {
    return reinterpret_cast<int*>(g_units.data());
}

}

EM_JS(int, js_update_units, (), {

    const win = window;
    const pole = win.stage?.[win.war_scr];

    if (!pole || !pole.obj) {
        return 0;
    }

    const units = Object.values(pole.obj);
    const count = units.length;

    // resize vector заранее
    Module._resize_units(count);

    const ptr = Module._get_units_buffer_ptr();
    const mem = new Int32Array(Module.HEAP32.buffer, ptr, count * 2);

    for (let i = 0; i < count; i++) {
        mem[i*2]   = units[i].x | 0;
        mem[i*2+1] = units[i].y | 0;
    }

    return count;
});

extern "C" {

EMSCRIPTEN_KEEPALIVE
void resize_units(int count) {
    g_units.resize(count);
}

}

void tick() {
    js_update_units();
}

std::vector<Unit> getUnits() {
    return g_units;
}

EMSCRIPTEN_BINDINGS(my_module) {

    value_object<Unit>("Unit")
        .field("x", &Unit::x)
        .field("y", &Unit::y);

    register_vector<Unit>("VectorUnit");

    function("tick", &tick);
    function("getUnits", &getUnits);
}

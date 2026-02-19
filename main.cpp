#include <vector>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

struct Unit {
    int x;
    int y;
};

/*
    JS функция:
    Возвращает массив:
    [x1, y1, x2, y2, x3, y3 ...]
*/
EM_JS(int, get_units_count, (), {
    const win = window;
    const pole_imported = win.stage?.[win.war_scr];
    if (!pole_imported || !pole_imported.obj) {
        Module._units_buffer = [];
        return 0;
    }

    const units_imported = Object.values(pole_imported.obj);

    const flat = [];
    for (const u of units_imported) {
        flat.push(u.x | 0);
        flat.push(u.y | 0);
    }

    Module._units_buffer = flat;

    return units_imported.length;
});

EM_JS(int, get_unit_data, (int index), {
    return Module._units_buffer[index] | 0;
});

std::vector<Unit> getUnitsFromPage() {

    std::vector<Unit> result;

    int count = get_units_count();

    for (int i = 0; i < count; i++) {
        Unit u;
        u.x = get_unit_data(i * 2);
        u.y = get_unit_data(i * 2 + 1);
        result.push_back(u);
    }

    return result;
}

EMSCRIPTEN_BINDINGS(my_module) {

    value_object<Unit>("Unit")
        .field("x", &Unit::x)
        .field("y", &Unit::y);

    register_vector<Unit>("VectorUnit");

    function("getUnitsFromPage", &getUnitsFromPage);
}

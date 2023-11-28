// Scene setup ============================================================

GSphere sphere --> GG.scene();
GCube cube --> GG.scene();

@(-1, 0, 0) => sphere.translate;
@(1, 0, 0) => cube.translate;

@(5, 5, 5) => sphere.mat().color;
@(0, 5, 0) => cube.mat().color;

// FX Chain ===============================================================
// GG.renderPass() --> BloomFX bloom --> OutputFX output;
PassThroughFX passthrough;
BloomFX bloom; OutputFX output;

GG.renderPass().next(passthrough).next(bloom).next(output);

// UI =====================================================================
UI_Window window;
window.text("bloom test");
fun void CheckboxListener(UI_Checkbox @ checkbox, FX @ effect)
{
    while (true)
    {
        checkbox => now;
        effect.bypass(checkbox.val());
    }
}

fun void GammaListener(UI_SliderFloat @ gammaSlider, OutputFX @ output) {
    while (true) {
        gammaSlider => now;
        gammaSlider.val() => output.gamma;
        <<< "gamma set to", output.gamma() >>>;
    }
}

fun void ExposureListener(UI_SliderFloat @ exposureSlider, OutputFX @ output) {
    while (true) {
        exposureSlider => now;
        exposureSlider.val() => output.exposure;
        <<< "exposure set to", output.exposure() >>>;
    }
}
[
    "None",
    "Linear",
    "Reinhard",
    "Cineon",
    "ACES",
    "Uncharted"
] @=> string tonemapOptions[];

fun void TonemapListener(UI_Dropdown @ tonemapDropdown, OutputFX @ output) {
    while (true) {
        tonemapDropdown => now;
        tonemapDropdown.val() => int val;
        if (val == 0) {
            output.toneMap(OutputFX.TONEMAP_NONE);
        } else if (val == 1) {
            output.toneMap(OutputFX.TONEMAP_LINEAR);
        } else if (val == 2) {
            output.toneMap(OutputFX.TONEMAP_REINHARD);
        } else if (val == 3) {
            output.toneMap(OutputFX.TONEMAP_CINEON);
        } else if (val == 4) {
            output.toneMap(OutputFX.TONEMAP_ACES);
        } else if (val == 5) {
            output.toneMap(OutputFX.TONEMAP_UNCHARTED);
        }
    }
}

fun void BloomStrengthListener(UI_SliderFloat @ strengthSlider, BloomFX @ bloom) {
    while (true) {
        strengthSlider => now;
        strengthSlider.val() => bloom.strength;
        <<< "bloom strength set to", bloom.strength() >>>;
    }
}

fun void BloomRadiusListener(UI_SliderFloat @ radiusSlider, BloomFX @ bloom) {
    while (true) {
        radiusSlider => now;
        radiusSlider.val() => bloom.radius;
        <<< "bloom radius set to", bloom.radius() >>>;
    }
}

fun void BloomThresholdListener(UI_SliderFloat @ thresholdSlider, BloomFX @ bloom) {
    while (true) {
        thresholdSlider => now;
        thresholdSlider.val() => bloom.threshold;
        <<< "bloom threshold set to", bloom.threshold() >>>;
    }
}

fun void BloomLevelsListener(UI_SliderInt @ levelsSlider, BloomFX @ bloom) {
    while (true) {
        levelsSlider => now;
        levelsSlider.val() => bloom.levels;
        <<< "bloom levels set to", bloom.levels() >>>;
    }
}

fun void BloomBlendListener(UI_Dropdown @ blendDropdown, BloomFX @ bloom) {
    while (true) {
        blendDropdown => now;
        blendDropdown.val() => int val;
        if (val == 0) {
            bloom.blend(BloomFX.BLEND_MIX);
        } else if (val == 1) {
            bloom.blend(BloomFX.BLEND_ADD);
        } 
    }
}


fun void KarisEnabledListener(UI_Checkbox @ karisCheckbox, BloomFX @ bloom) {
    while (true) {
        karisCheckbox => now;
        karisCheckbox.val() => bloom.karisAverage;
    }
}

GG.renderPass().next() @=> FX @ effect;

0 => int effectIndex;
while (effect != null) {
    // effect.UI() @=> UI_Element @ effectUI;
    // effectUI.text("pass " + effectIndex);
    // window.add(effectUI);

    <<< "Creating UI for pass", effectIndex, "Type:", Type.of(effect).baseName() >>>;

    UI_Checkbox checkbox;
    checkbox.text("pass " + effectIndex);
    checkbox.val(false);    
    window.add(checkbox);
    spork ~ CheckboxListener(checkbox, effect);

    // create UI based on type
    Type.of(effect).baseName() => string baseName;
    if (baseName == "OutputFX") {
        // gamma
        UI_SliderFloat gammaSlider;
        gammaSlider.text("Gamma");
        gammaSlider.val((effect$OutputFX).gamma());
        <<< "init gamma: ", (effect$OutputFX).gamma() >>>;
        gammaSlider.range(0.1, 10);
        window.add(gammaSlider);
        spork ~ GammaListener(gammaSlider, effect$OutputFX);

        // exposure
        UI_SliderFloat exposureSlider;
        exposureSlider.text("Exposure");
        exposureSlider.val(1);
        exposureSlider.range(0.01, 16);
        window.add(exposureSlider);
        spork ~ ExposureListener(exposureSlider, effect$OutputFX);
        
        // tonemap
        UI_Dropdown tonemapDropdown;
        tonemapDropdown.text("Tone Mapping method");
        tonemapDropdown.options(tonemapOptions);
        tonemapDropdown.val(output.toneMap());
        window.add(tonemapDropdown);
        spork ~ TonemapListener(tonemapDropdown, effect$OutputFX);

    } else if (baseName == "BloomFX") {
        // strength
        UI_SliderFloat strengthSlider;
        strengthSlider.text("Bloom Strength");
        strengthSlider.val((effect$BloomFX).strength());
        strengthSlider.range(0, 2);
        window.add(strengthSlider);
        spork ~ BloomStrengthListener(strengthSlider, effect$BloomFX);

        // radius
        UI_SliderFloat radiusSlider;
        radiusSlider.text("Bloom Radius");
        radiusSlider.val((effect$BloomFX).radius());
        radiusSlider.range(0.001, .01);
        window.add(radiusSlider);
        spork ~ BloomRadiusListener(radiusSlider, effect$BloomFX);

        // threshold
        UI_SliderFloat thresholdSlider;
        thresholdSlider.text("Bloom Threshold");
        thresholdSlider.val((effect$BloomFX).threshold());
        thresholdSlider.range(0, 2);
        window.add(thresholdSlider);
        spork ~ BloomThresholdListener(thresholdSlider, effect$BloomFX);

        // levels
        UI_SliderInt levelsSlider;
        levelsSlider.text("Bloom #Passes");
        levelsSlider.val((effect$BloomFX).levels());
        levelsSlider.range(1, 10);
        window.add(levelsSlider);
        spork ~ BloomLevelsListener(levelsSlider, effect$BloomFX);

        // blend mode
        UI_Dropdown blendDropdown;
        blendDropdown.text("Bloom Blend Mode");
        ["mix", "add"] @=> string blendOptions[];
        blendDropdown.options(blendOptions);
        blendDropdown.val(0);
        window.add(blendDropdown);
        spork ~ BloomBlendListener(blendDropdown, effect$BloomFX);

        // karis enabled
        UI_Checkbox karisCheckbox;
        karisCheckbox.text("Karis Bloom");
        window.add(karisCheckbox);
        spork ~ KarisEnabledListener(karisCheckbox, effect$BloomFX);

    }

    effect.next() @=> effect;
    effectIndex++;
}

// Main loop ==============================================================
while (true) { GG.nextFrame() => now; }
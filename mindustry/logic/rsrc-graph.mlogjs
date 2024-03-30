const version: String = "Rsrc-20240325-5";
const display_version = "[violet]" + version + "[white]";

var alert = 0.3;    // 10%
var split_mode = -1; // -1 auto, 0 split horiz, 1 split vertically

var alert_sw: AnyBuilding = undefined;
var node;
var display : AnyBuilding;
var message : AnyBuilding;
var size;

print(display_version+"\n");

for (let i = Vars.links - 1; i >= 0; i--) {
    let link = getLink(i);
    let ltype = link.type;

    if (ltype == Blocks.logicDisplay) {
        size = 80;
        display = link;
    }
    else if (ltype == Blocks.largeLogicDisplay) {
        size = 176;
        display = link;
    }
    else if (ltype == Blocks.switch)
        alert_sw = link;
    else if (ltype == Blocks.powerNode || ltype == Blocks.powerNodeLarge) {
        node = link;
    } else if (ltype == Blocks.message)
        message = link;
}
var max_rsrc;
var links = Vars.links;
var x = 0;
var dx = 1;
var dy = 1;
var y1 = 0;
var n = 0;
var bars = 0;
var bar;
var barh = size;
var vault = 0;


const drawBar = function (r, g, b, rsrc) {
    if (rsrc == 0) return;
    bar = bar + 1;
    draw.color(r, g, b);
    var pct = rsrc / max_rsrc;
    var height = Math.ceil(barh * pct);
    var width = dx + 1;
    draw.rect({ x: x, y: y1, width: width, height: height });
    if (pct < alert && (alert_sw === undefined || alert_sw.enabled == true)) {
        var w = Math.max(2, Math.min(dx / 3, barh/10, 10));
        var px = x + (dx + 1 - w) / 2;
        var py = y1 + 0.2 * barh;
        draw.color(255, 0, 0);
        draw.rect({ x: px, y: py, width: w, height: w });
        py += 2 * w;
        var h = 3 * w;
        draw.rect({ x: px, y: py, width: w, height: h });
    }
    x = x + dx;
}

while (links == Vars.links) {
    while (links == Vars.links) {
        draw.clear(0, 0, 0);
        x = 0;
        if (split_mode == 0) {
            barh = size / vault;
        }
        else
            barh = size;

        vault = 0;

        dx = size / bars;
        bars = 0;
        bar = 0;

        y1 = 0;

        for (let i = Vars.links - 1; i >= 0; i--) {
            let link = getLink(i);
            let ltype = link.type;

            if (ltype == Blocks.logicDisplay
                || ltype == Blocks.largeLogicDisplay
                || ltype == Blocks.message
                || ltype == Blocks.switch) {
            }
            else {
                vault = vault + 1;
                draw.color(255, 255, 255);
                if (split_mode == 0) {
                    draw.line({ x: 0, y: y1, x2: size, y2: y1 });
                    bar = 0;
                    x = 0;
                }
                else
                    draw.line({ x: x, y: 0, x2: x, y2: size });
                var old_bar = bar;

                if (ltype == Blocks.liquidContainer
                    || ltype == Blocks.liquidTank) {
                    max_rsrc = link.liquidCapacity;
                }
                else {
                    max_rsrc = link.itemCapacity;
                }

                if (true) {
                    var max = 0;
                    if (link.itemCapacity !== undefined)
                        max = link.itemCapacity;
                    else if (link.liquidCapacity !== undefined)
                        max = link.liquidCapacity;
                    else if (link.ammoCapacity != undefined)
                        max = link.ammoCapacity;
                    print(ltype, " max=", max);
                }
                
                drawBar(0xd8, 0x9d, 0x73, link.copper);
                drawBar(0x93, 0x85, 0xb2, link.lead);
                drawBar(255, 255, 255, link.metaglass);
                drawBar(0x95, 0xab, 0Xd8, link.graphite);
                drawBar(0xf7, 0xcb, 0xa4, link.sand);
                drawBar(0x40, 0x40, 0x40, link.coal);
                drawBar(0xa4, 0xb8, 0xfa, link.titanium);
                drawBar(0xff, 0x7d, 0xb4, link.thorium);
                drawBar(0x9b, 0x92, 0x8b, link.scrap);
                drawBar(0x4f, 0x50, 0x62, link.silicon);
                drawBar(0xcb, 0xd8, 0x7e, link.plastanium);
                drawBar(0xf1, 0x95, 0x83, link.phaseFabric);
                drawBar(0xe7, 0xc1, 0x24, link.surgeAlloy);
                drawBar(0x62, 0x30, 0xff, link.sporePod);
                drawBar(0xff, 0x3e, 0x2b, link.blastCompound);
                drawBar(0xff, 0x86, 0x37, link.pyratite);
                drawBar(0x59, 0x6a, 0xb8, link.water);
                drawBar(0x6e, 0xcd, 0xec, link.cryofluid);
                drawBar(0x6b, 0x67, 0x5f, link.oil);
                drawBar(0xcf, 0x9f, 0x00, link.slag);

                if (split_mode == 1 && old_bar == bar)
                    drawBar(0, 0, 0, 1);
                if (split_mode == 0)
                    y1 += barh;
                if (bar > bars)
                    bars = bar;
                // drawFlush(display);
            }
        }
        if (split_mode == -1) {
            if (bars > vault)
                split_mode = 0;
            else
                split_mode = 1;
        }
        drawFlush(display);

        print(display_version);
        if (vault === 0) {
            print("[red]Connect a resource container please[white]\n");
        }
        if (display === undefined) {
            print("[red]Connect a display please.[white]");
        }

        printFlush(message);
    }
}
printFlush(message);


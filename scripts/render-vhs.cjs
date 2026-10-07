const {createCanvas,loadImage}=require('@napi-rs/canvas');
const fs=require('fs');
const W=384,H=288,CW=64;
const toile=(w,h)=>createCanvas(w,h);
function bruit(){const c=toile(W,H),g=c.getContext('2d'),im=g.createImageData(W,H);for(let i=0;i<im.data.length;i+=4){const v=Math.random()*255;im.data[i]=im.data[i+1]=im.data[i+2]=v;im.data[i+3]=255;}g.putImageData(im,0,0);return c;}
function lineOffset(y, h, d) {
  let dx = Math.sin(d.t * 2.1 + y * 0.043) * 1.1 + Math.sin(d.t * 9.7 + y * 0.29) * 0.5;
  // Tension de la bande : le haut de l'image se tord.
  if (y < 22) dx += (22 - y) * 0.28 * (0.6 + 0.4 * Math.sin(d.t * 0.9));
  if (d.tracking !== null) {
    const dist = Math.abs(y - d.tracking);
    if (dist < 16) dx += (d.alea - 0.5) * 26 * d.force * (1 - dist / 16);
  }
  if (y >= h - 10) dx += 5 + (d.alea - 0.5) * 12 * ((y - (h - 10)) / 10 + 0.4);
  return dx;
}

async function main(){
const base=toile(W,H),b=base.getContext('2d');
const luma=toile(W,H),l=luma.getContext('2d');
const chroma=toile(CW,H),c=chroma.getContext('2d');
const trame=toile(W,H),f=trame.getContext('2d');
const out=toile(W,H),o=out.getContext('2d');
const bruits=[bruit(),bruit(),bruit(),bruit()];
const reduit=false,glitch=false,p={lecture:true};
let vague=null;
const dir=process.argv[2];const files=fs.readdirSync(dir).filter(x=>x.endsWith('.png')).sort();
for(let i=0;i<files.length;i++){
const n=i+1,t=i/25,ts=t*1000;
let tracking=(t>=1.25&&t<2.65)?{debut:1250,duree:1400,force:0.55}:null;
b.globalCompositeOperation='copy';b.drawImage(await loadImage(dir+'/'+files[i]),0,0,W,H);
      // 2. Luminance nette, chrominance étalée et décalée, écho du signal
      l.filter = "grayscale(1) contrast(1.16) brightness(1.06) blur(0.45px)";
      l.drawImage(base, 0, 0);
      l.filter = "none";
      c.filter = "saturate(1.5) blur(0.6px)";
      c.drawImage(base, 0, 0, CW, H);
      c.filter = "none";
      f.globalCompositeOperation = "copy";
      f.drawImage(luma, 0, 0);
      f.globalCompositeOperation = "color";
      f.drawImage(chroma, 0, 0, CW, H, 4, 0, W, H);
      // bavure rouge qui déborde à droite des aplats
      f.globalCompositeOperation = "screen";
      f.globalAlpha = 0.16;
      f.drawImage(chroma, 0, 0, CW, H, 11, 1, W, H);
      // accentuation des contours du magnétoscope : halo clair juste après les bords
      f.globalCompositeOperation = "overlay";
      f.globalAlpha = 0.3;
      f.drawImage(luma, 2, 0);
      // écho du signal (image fantôme décalée)
      f.globalCompositeOperation = "lighter";
      f.globalAlpha = 0.06;
      f.drawImage(luma, 10, 0);
      f.globalAlpha = 0.03;
      f.drawImage(luma, 21, 0);
      // dominante chaude des cassettes fatiguées
      f.globalCompositeOperation = "soft-light";
      f.globalAlpha = 1;
      f.fillStyle = "rgba(255, 130, 70, 0.22)";
      f.fillRect(0, 0, W, H);
      f.globalAlpha = 1;
      f.globalCompositeOperation = "source-over";

      // 3. Lignes : ondulation, tracking, commutation des têtes, saut vertical
      o.globalCompositeOperation = "source-over";
      o.globalAlpha = 1;
      o.fillStyle = "#050308";
      o.fillRect(0, 0, W, H);
      let bande = null;
      let force = 0;
      if (tracking) {
        const k = (ts - tracking.debut) / tracking.duree;
        if (k >= 1) tracking = null;
        else {
          bande = H + 20 - k * (H + 40);
          force = tracking.force;
        }
      }
      const pause = !p.lecture;
      const saut = glitch ? Math.round((Math.random() - 0.5) * 14) : pause ? (n % 2) - 0.5 : 0;
      const kv = vague ? (ts - vague.debut) / vague.duree : -1;
      if (kv >= 1) vague = null;
      for (let y = 0; y < H; y += 2) {
        const ondule = kv >= 0 && kv < 1 ? Math.sin(y * 0.08 + ts * 0.02) * 7 * Math.sin(Math.PI * kv) : 0;
        const dx = reduit
          ? 0
          : ondule + lineOffset(y, H, { t, tracking: bande, force, alea: Math.random() }) + (pause && (Math.abs(y - H * 0.34) < 5 || Math.abs(y - H * 0.72) < 4) ? (Math.random() - 0.5) * 30 : 0);
        o.drawImage(trame, 0, y, W, 2, dx, y + saut, W, 2);
      }

      // 4. Bruits : bande de tracking, commutation des têtes, barres de pause, drop-outs, grain
      o.globalCompositeOperation = "screen";
      const bandeBruit = (y, h, a) => {
        o.globalAlpha = a;
        o.drawImage(bruits[(n + 1) % 4], Math.floor(Math.random() * 40), Math.floor(Math.random() * H), W, h, 0, y, W, h);
      };
      if (bande !== null) bandeBruit(Math.round(bande - 5), 10, 0.55 * force + 0.15);
      bandeBruit(H - 7, 7, 0.45);
      if (pause) {
        bandeBruit(Math.round(H * 0.34 - 3 + (n % 3)), 6, 0.6);
        bandeBruit(Math.round(H * 0.72 - 2 - (n % 2)), 4, 0.5);
      }
      if (glitch) bandeBruit(0, H, 0.35);
      o.globalAlpha = 1;
      if (!reduit && Math.random() < 0.45) {
        o.fillStyle = "rgba(255,255,255,0.8)";
        for (let i = 0, k = 1 + Math.floor(Math.random() * 4); i < k; i++) o.fillRect(Math.random() * W, Math.random() * H, 6 + Math.random() * 90, 1);
      }
      if (!reduit && Math.random() < 0.08) bandeBruit(Math.floor(Math.random() * H), 2 + Math.floor(Math.random() * 3), 0.5);
      o.globalCompositeOperation = "overlay";
      o.globalAlpha = 0.18;
      o.drawImage(bruits[n % 4], reduit ? 0 : -Math.floor(Math.random() * 20), reduit ? 0 : -Math.floor(Math.random() * 20));
      // noirs délavés, légèrement violacés
      o.globalAlpha = 1;
      o.globalCompositeOperation = "screen";
      o.fillStyle = "rgba(34, 20, 46, 0.14)";
      o.fillRect(0, 0, W, H);
      // Pas de lignes de balayage ni de bombé : c'est l'écran du moniteur qui fait le tube.
      o.globalCompositeOperation = "source-over";


const dst=toile(768,576),d=dst.getContext('2d');d.imageSmoothingEnabled=true;d.drawImage(out,0,0,768,576);
fs.writeFileSync(dir+'/../vhs/'+files[i],dst.toBuffer('image/png'));
}
}
main().catch(e=>{console.error(e);process.exit(1)});

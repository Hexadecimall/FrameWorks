const state = document.querySelector('#engineState');
const result = document.querySelector('#result');

const response = await fetch('frameworks.wasm');
if (!response.ok) throw new Error(`WASM request failed: ${response.status}`);
const bytes = await response.arrayBuffer();
const { instance } = await WebAssembly.instantiate(bytes, {});
const engine = instance.exports;

const describe = label => {
  const kind = engine.fw_document_kind() === 0 ? 'blank document' : 'Orbital Poster';
  result.value = `${label}: ${kind}, ${engine.fw_document_width()} × ${engine.fw_document_height()}`;
};

state.textContent = `WASM engine ${engine.fw_engine_version()} ready`;
state.classList.add('ready');
describe('Loaded');

const createDocument = () => {
  engine.fw_new_document();
  describe('Created');
};
document.querySelector('#newDocument').addEventListener('click', createDocument);
document.querySelector('#newSide').addEventListener('click', createDocument);
document.querySelector('#recentDocument').addEventListener('click', () => {
  engine.fw_open_starter();
  describe('Opened');
});

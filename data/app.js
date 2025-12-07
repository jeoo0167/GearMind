document.addEventListener('DOMContentLoaded', () => {

  const panelBtn = document.getElementById('panelBtn');
  const sidePanel = document.getElementById('sidePanel');
  const batteryEl = document.getElementById('batteryLevel');
  const submitBtn = document.getElementById('submitBtn');

  const ssidInput = document.getElementById('ssidInput');
  const passwordInput = document.getElementById('passwordInput');
  const soundsToggle = document.getElementById('soundsToggle');

  const ranges = Array.from(document.querySelectorAll('.r'));
  const nums   = Array.from(document.querySelectorAll('.n'));

  /* Panel toggle */
  panelBtn.addEventListener('click', () => {
    const open = sidePanel.classList.toggle('open');
    sidePanel.setAttribute('aria-hidden', !open);
  });

  /* Remote control buttons */
  sidePanel.querySelectorAll('button[data-cmd]').forEach(btn => {
    btn.addEventListener('click', () => {
      const cmd = btn.getAttribute('data-cmd');
      console.log("Remote command:", cmd);

      // ESP32 endpoint opcional:
      // fetch(`/cmd?m=${cmd}`)
    });
  });

  /* Slider sync */
  ranges.forEach((r, i) => r.addEventListener('input', () => {
    if (nums[i]) nums[i].value = r.value;
  }));

  nums.forEach((n, i) => n.addEventListener('input', () => {
    if (ranges[i]) ranges[i].value = n.value;
  }));


  /* Load config.json */
  async function loadConfig() {
    try {
      const res = await fetch('/config.json', { cache: 'no-store' });
      if (!res.ok) throw new Error('HTTP ' + res.status);

      const cfg = await res.json();
      console.log("Config loaded:", cfg);

      const thresholds = cfg.thresholds || [50, 50, 50, 50];
      const timers     = cfg.timers     || [1000, 1000, 1000];
      const speeds     = cfg.speeds     || [120, 120];

      const all = thresholds.concat(timers).concat(speeds);

      all.forEach((val, i) => {
        if (ranges[i]) ranges[i].value = val;
        if (nums[i]) nums[i].value = val;
      });

      ssidInput.value     = cfg.network?.ssid || "";
      passwordInput.value = cfg.network?.password || "";
      soundsToggle.checked = cfg.network?.sounds_enabled || false;

      if (cfg.battery?.level)
        batteryEl.textContent = cfg.battery.level + "%";

    } catch (err) {
      console.error("Error loading config:", err);
    }
  }


  /* Submit configuration */
  async function submitConfig(event) {
    event?.preventDefault();

    const thresholds = ranges.slice(0,4).map(r => Number(r.value));
    const timers = ranges.slice(4,7).map(r => Number(r.value));
    const speeds = ranges.slice(7,9).map(r => Number(r.value));

    const payload = {
      network: {
        ssid: ssidInput.value,
        password: passwordInput.value,
        sounds_enabled: soundsToggle.checked
      },
      thresholds,
      timers,
      speeds
    };

    try {
      const res = await fetch('/saveConfig', {
        method: "POST",
        headers: {"Content-Type": "application/json"},
        body: JSON.stringify(payload)
      });

      if (!res.ok) throw new Error("HTTP " + res.status);

      console.log("Config saved!");
      submitBtn.textContent = "Saved ✓";
      setTimeout(() => submitBtn.textContent = "Submit", 1200);

    } catch (err) {
      console.error("Error saving config:", err);
      submitBtn.textContent = "Error";
      setTimeout(() => submitBtn.textContent = "Submit", 1200);
    }
  }

  submitBtn.addEventListener("click", submitConfig);

  /* Load initial config */
  loadConfig();
});

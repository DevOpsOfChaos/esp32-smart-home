const sharedCss = `
<style>
  .sm-panel {
    background: linear-gradient(180deg, #fffdf9 0%, #f9f3ea 100%);
    border: 1px solid #d7c9b7;
    border-radius: 18px;
    padding: 16px;
    box-shadow: 0 10px 28px rgba(59, 41, 20, 0.08);
  }
  .sm-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 12px;
  }
  .sm-card {
    background: rgba(255, 255, 255, 0.92);
    border: 1px solid #eadfce;
    border-radius: 14px;
    padding: 12px;
  }
  .sm-card h3,
  .sm-panel h3 {
    margin: 0 0 6px 0;
    font-size: 1rem;
    color: #1d4d4f;
  }
  .sm-kpi {
    background: #0f766e;
    color: #fffdf7;
    border-radius: 14px;
    padding: 14px;
  }
  .sm-kpi span {
    display: block;
    font-size: 0.82rem;
    opacity: 0.82;
  }
  .sm-kpi strong {
    display: block;
    font-size: 1.75rem;
    margin-top: 4px;
  }
  .sm-list {
    display: grid;
    gap: 12px;
  }
  .sm-item {
    background: rgba(255, 255, 255, 0.92);
    border: 1px solid #eadfce;
    border-radius: 14px;
    padding: 12px;
    display: grid;
    gap: 6px;
  }
  .sm-meta {
    display: flex;
    flex-wrap: wrap;
    gap: 8px;
  }
  .sm-badge {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    padding: 4px 10px;
    border-radius: 999px;
    font-size: 0.78rem;
    font-weight: 600;
    background: #efe6d8;
    color: #5b4b39;
  }
  .sm-badge.online {
    background: #d9f4ea;
    color: #166c53;
  }
  .sm-badge.offline {
    background: #f6ddd7;
    color: #8b3021;
  }
  .sm-muted {
    color: #685d51;
    font-size: 0.88rem;
  }
  .sm-stack {
    display: grid;
    gap: 10px;
  }
  .sm-form {
    display: grid;
    gap: 10px;
  }
  .sm-form label {
    display: grid;
    gap: 4px;
    font-size: 0.86rem;
    color: #4d443b;
  }
  .sm-form input,
  .sm-form textarea,
  .sm-form select,
  .sm-form button {
    border-radius: 12px;
    border: 1px solid #d7c9b7;
    padding: 10px 12px;
    font: inherit;
  }
  .sm-form textarea {
    min-height: 110px;
    resize: vertical;
  }
  .sm-form button {
    background: linear-gradient(135deg, #0f766e 0%, #14532d 100%);
    color: #fffdf7;
    cursor: pointer;
    font-weight: 700;
  }
  .sm-form button.alt {
    background: linear-gradient(135deg, #7c4d12 0%, #b45309 100%);
  }
  .sm-actions {
    display: flex;
    flex-wrap: wrap;
    gap: 10px;
  }
  .sm-code {
    font-family: "IBM Plex Mono", Consolas, monospace;
    font-size: 0.82rem;
    background: #f4ede2;
    border-radius: 10px;
    padding: 8px 10px;
    word-break: break-word;
  }
  .sm-note {
    margin: 0;
    padding: 10px 12px;
    border-radius: 12px;
    background: #f4ede2;
    color: #5b4b39;
    font-size: 0.88rem;
  }
  .sm-two-col {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    gap: 12px;
  }
  .sm-empty {
    margin: 0;
    padding: 12px;
    border-radius: 12px;
    background: #f4ede2;
    color: #5b4b39;
  }
  @media (max-width: 700px) {
    .sm-panel {
      padding: 14px;
      border-radius: 14px;
    }
  }
</style>`;

function node(id, type, extra) {
  return { id, type, ...extra };
}

function tab(id, label) {
  return node(id, "tab", { label, disabled: false, info: "" });
}

function uiPage(id, name, path, icon, order) {
  return node(id, "ui-page", {
    ui: "1000000000000200",
    path,
    name,
    icon,
    layout: "grid",
    theme: "1000000000000201",
    breakpoint: "sm",
    order,
    className: "",
    visible: "true",
    disabled: "false"
  });
}

function uiGroup(id, page, name, width, order) {
  return node(id, "ui-group", {
    page,
    name,
    width,
    height: "1",
    order,
    showTitle: true,
    className: "",
    visible: "true",
    disabled: "false",
    groupType: "default"
  });
}

function uiTemplate(id, group, name, order, format, wires = []) {
  return node(id, "ui-template", {
    z: "1000000000000002",
    group,
    name,
    order,
    width: "12",
    height: "0",
    head: "",
    format,
    storeOutMessages: false,
    passthru: false,
    resendOnRefresh: true,
    templateScope: "local",
    className: "",
    x: 1180,
    y: 0,
    wires
  });
}

function commandForm(title, commandType, note) {
  return `${sharedCss}
<template>
  <div class='sm-panel'>
    <h3>${title}</h3>
    <p class='sm-note'>${note}</p>
    <div class='sm-form'>
      <label>
        Zielgeraet (node_id)
        <input v-model='form.device_id' placeholder='net_erl_01'>
      </label>
      <label>
        Payload als JSON oder Text
        <textarea v-model='form.payloadText' placeholder='{"channel":"relay1","value":true}'></textarea>
      </label>
      <button @click='sendCommand'>An MQTT senden</button>
      <p class='sm-muted'>Topic: <span class='sm-code'>smarthome/node/&lt;node_id&gt;/${commandType}</span></p>
      <p v-if='feedback' class='sm-muted'>{{ feedback }}</p>
    </div>
  </div>
</template>
<script>
export default {
  data() {
    return {
      form: {
        device_id: '',
        payloadText: '{}'
      },
      feedback: ''
    }
  },
  methods: {
    sendCommand() {
      if (!this.form.device_id) {
        this.feedback = 'node_id fehlt'
        return
      }
      this.feedback = 'Gesendet an ' + this.form.device_id
      this.send({
        topic: 'ui/mqtt/action',
        commandType: '${commandType}',
        payload: {
          device_id: this.form.device_id.trim(),
          payload_text: this.form.payloadText
        }
      })
    }
  }
}
</script>`;
}

function auditTemplate(title, limit) {
  return `${sharedCss}
<template>
  <div class='sm-panel'>
    <h3>${title}</h3>
    <div class='sm-list' v-if='rows.length'>
      <article class='sm-item' v-for='row in rows.slice(0, ${limit})' :key='row.id'>
        <div class='sm-meta'>
          <span class='sm-badge'>{{ row.direction }}</span>
          <span class='sm-badge'>{{ row.channel }}</span>
          <span class='sm-badge' v-if='row.device_id'>{{ row.device_id }}</span>
          <span class='sm-muted'>{{ row.occurred_at }}</span>
        </div>
        <div class='sm-code'>{{ row.topic }}</div>
        <div class='sm-muted'>{{ row.payload_preview }}</div>
      </article>
    </div>
    <p class='sm-empty' v-else>Noch keine Audit-Eintraege.</p>
  </div>
</template>
<script>
export default {
  computed: {
    rows() {
      return Array.isArray(this.msg?.payload) ? this.msg.payload : []
    }
  }
}
</script>`;
}

const overviewTemplate = `${sharedCss}
<template>
  <div class='sm-panel sm-stack'>
    <div class='sm-grid'>
      <section class='sm-kpi'>
        <span>Geraete</span>
        <strong>{{ total }}</strong>
      </section>
      <section class='sm-kpi'>
        <span>Online</span>
        <strong>{{ online }}</strong>
      </section>
      <section class='sm-kpi'>
        <span>Offline</span>
        <strong>{{ offline }}</strong>
      </section>
      <section class='sm-kpi'>
        <span>Ohne Raum</span>
        <strong>{{ unassigned }}</strong>
      </section>
    </div>
    <div class='sm-list' v-if='devices.length'>
      <article class='sm-item' v-for='device in devices.slice(0, 6)' :key='device.device_id'>
        <div class='sm-meta'>
          <span :class="['sm-badge', device.is_online ? 'online' : 'offline']">{{ device.is_online ? 'online' : 'offline' }}</span>
          <span class='sm-badge'>{{ device.device_role }}</span>
          <span class='sm-badge'>{{ device.room_label }}</span>
        </div>
        <h3>{{ device.display_name }}</h3>
        <div class='sm-muted'>{{ device.device_id }} | {{ device.device_class || 'unknown' }}</div>
        <div class='sm-muted'>{{ device.state_preview || device.status_preview || 'Noch kein state/status im Speicher.' }}</div>
      </article>
    </div>
    <p class='sm-empty' v-else>Noch keine Geraete erkannt.</p>
  </div>
</template>
<script>
export default {
  computed: {
    devices() {
      return Array.isArray(this.msg?.payload) ? this.msg.payload : []
    },
    total() {
      return this.devices.length
    },
    online() {
      return this.devices.filter((device) => device.is_online).length
    },
    offline() {
      return this.devices.filter((device) => !device.is_online).length
    },
    unassigned() {
      return this.devices.filter((device) => device.room_label === 'Ohne Raum').length
    }
  }
}
</script>`;

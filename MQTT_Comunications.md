It has already been initialized.

```seqdiag
seqdiag {
  HomeAssistant -> MQTTServer [label = "Subscribe to topic (remote/status)"];
  HomeAssistant <-- MQTTServer [label = "Receive message (remote/status)"];
  
  HomeAssistant -> MQTTServer [label = "Publish message (remote/rf)"];
  
  MQTTServer --> RP2040 [label = "Receive message (remote/rf)"];
  
  RP2040 -> CC1101 [label = "Send RF parameters"];
  RP2040 <-- CC1101 [label = "Receive RF parameters"];
  
  MQTTServer <- RP2040 [label = "Publish to topic (remote/status)"];
  
  HomeAssistant <-- MQTTServer [label = "Receive message (remote/status)"];
  
  HomeAssistant -> MQTTServer [label = "Publish message (remote/status)"];
}

```

Diagram made with [SeqDiagram](https://github.com/pomdtr/vscode-markdown-kroki)

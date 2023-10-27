It has already been initialized.

```seqdiag
seqdiag {
  
  RP2040 -> CC1101 [label = "Send RF parameters"];
  RP2040 <-- CC1101 [label = "Receive RF parameters"];
  
}

```
Diagram made with [SeqDiagram](https://github.com/pomdtr/vscode-markdown-kroki)

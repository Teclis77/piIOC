# close_window.py
# Chiude la finestra corrente in CSS Phoebus
# Collega questo script a un ActionButton con action "Execute Script"
#
# Configurazione nel Display Builder:
#   Widget: Action Button
#   Action: Execute Script -> close_window.py
#   Nessuna PV necessaria

from org.csstudio.display.builder.runtime.script import ScriptUtil

# Risale al display root e lo chiude
ScriptUtil.closeDisplay(widget)
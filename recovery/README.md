# Recuperación local

Los dos binarios `.bin` estaban presentes al iniciar esta refactorización. Sus SHA-256 coinciden con el informe anterior aportado por Ernesto, que indica generación con Builder legado 2025.2.2 desde `esp-roomba.yaml`, sin instalación.

`manifest.json` distingue evidencia aportada de verificaciones realizadas ahora. No existe registro local del commit externo usado en esa compilación; no se ha demostrado identidad exacta fuente→binario ni restauración en hardware. El tag ya decía "compiled firmware not yet verified"; no se ha movido ni reescrito.

Los `.bin` pueden incorporar secretos del dispositivo. Permanecen excluidos de Git y de cualquier paquete público. No se ha copiado `secrets.yaml`. El manifiesto y SHA256SUMS no contienen credenciales.

Ver [procedimiento completo](../docs/restore.md). No confundir los binarios originales con pruebas compiladas con claves ficticias en `build/`.

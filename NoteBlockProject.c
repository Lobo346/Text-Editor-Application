/*Programa hecho por Aoshi Blanco C.I 30.243.721 Universidad Santa Maria
Bloc de Notas con Interfaz, Corrector Ortografico, Justificado, limite de lineas
*/
// Incluimos la libreria de la Interfaz grafica
#include <gtk/gtk.h>
#include<gdk/gdk.h>
#include<string.h>
#include<ctype.h>
#include <locale.h>
#include <windows.h>
#include <hunspell/hunspell.h>
#include <glib.h>

//Variables Globales que necesitamos en nuestro codigo
GtkWidget *text_views[4];
int contador=0;
gboolean was_period = FALSE;

//===========================================PROTOTIPOS DE FUNCIONES===================================
void borrar(GtkWidget *button, gpointer data);

static void limite_texto(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer data);

static void mayus(GtkTextBuffer *buffer,
               GtkTextIter   *location,
               gchar         *text,
               gint           len,
               gpointer       user_data);


void mover_cursor(GtkTextView *text_view);

gboolean linea_vacia(gpointer data);

void buffer_borrar(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end, gpointer data);

static void buffer_cambio(GtkTextBuffer *buffer, gpointer data);

void opcion_ayuda(GtkButton *button, gpointer user_data);

void opcion_info(GtkButton *button, gpointer user_data);

void opcion_txt(GtkButton *button, gpointer user_data);

void prevent_pegado(GtkTextView *text_view, gpointer user_data);

static void sugerencia(GtkButton *button, gpointer user_data);

void corrector(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data);

void leyenda(GtkButton *button, gpointer user_data);

void encriptado(GtkButton *button, gpointer user_data);

void desencriptado(GtkButton *button, gpointer user_data);

void opcion_encript(GtkButton *button, gpointer user_data);

//===========================================FUNCION PRINCIPAL=========================================================

//Esta es nuestra funcion principal, donde creamos la app, y declaramos todas los widgets a usar
static void activate (GtkApplication* app, gpointer user_data) {
	
    GtkWidget *window;					//Widgets: Window= Ventana Principal
    GtkWidget *scrolled_window;			//Scrolled Window= Hacemos que la Ventana principal tenga scroll
    GtkWidget *text_view;				//text_view= Nuestro Cuadro de texto
    GtkWidget *box;						//box= La caja donde el cuadro de texto estara situado
    GtkCssProvider *provider;			//provider= Funcion que nos ayuda a traer las opciones de estilo de CSS
    
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Bloc de Notas");
    gtk_window_set_default_size (GTK_WINDOW (window), 900, 600);

	GtkWidget *header_bar = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    scrolled_window = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_window_set_child (GTK_WINDOW (window), scrolled_window);
    
    GtkWidget *button = gtk_button_new_with_label("Opciones");
    gtk_widget_set_halign(button, GTK_ALIGN_START); // Ponemos el menu a la izquierda
    gtk_box_append(GTK_BOX(box), button);
     
    GtkWidget *popover = gtk_popover_new();
    gtk_widget_set_parent(popover, button);
    // Ponemos el menu a la derecha
	gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_RIGHT);

    // Creamos la caja donde esta el menu 
    GtkWidget *popover_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_popover_set_child(GTK_POPOVER(popover), popover_box);


    // Creamos los objetos
    GtkWidget *menu_item1 = gtk_button_new_with_label("Ayuda");
    	g_signal_connect(menu_item1, "clicked", G_CALLBACK(opcion_ayuda), NULL);
    GtkWidget *menu_item2 = gtk_button_new_with_label("Informacion");
    	g_signal_connect(menu_item2, "clicked", G_CALLBACK(opcion_info), NULL);
    GtkWidget *menu_item3 = gtk_button_new_with_label("Guardar en archivo .txt");
   		g_signal_connect(menu_item3, "clicked", G_CALLBACK(opcion_txt), NULL);
    GtkWidget *menu_item4 = gtk_button_new_with_label("Borrar Contenido");
   		g_signal_connect(menu_item4, "clicked", G_CALLBACK(borrar), NULL);
    GtkWidget *menu_item5 = gtk_button_new_with_label("Encriptado");
   		g_signal_connect(menu_item5, "clicked", G_CALLBACK(opcion_encript), NULL);
    
    gtk_box_append(GTK_BOX(popover_box), menu_item1);
    gtk_box_append(GTK_BOX(popover_box), menu_item2);
    gtk_box_append(GTK_BOX(popover_box), menu_item3);
    gtk_box_append(GTK_BOX(popover_box), menu_item4);
     gtk_box_append(GTK_BOX(popover_box), menu_item5);
    

    // conectamos la senal
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_popover_popup), popover);
	
	//Generamos los 4 text view
  for (int i=0; i<4; i++){
        GtkWidget *text_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
        
        gtk_widget_set_margin_top (text_box, 10);
        gtk_widget_set_margin_bottom (text_box, 10);
		
		GtkWidget *center_box = gtk_center_box_new();
		gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), text_box);
		gtk_box_append(GTK_BOX(box), center_box);


        text_view = gtk_text_view_new ();
        
       	gtk_text_view_set_justification(GTK_TEXT_VIEW(text_view), GTK_JUSTIFY_FILL);
       	
       	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
       	
        g_signal_connect(buffer, "changed", G_CALLBACK(buffer_cambio), NULL);
        g_signal_connect(buffer, "insert-text", G_CALLBACK(limite_texto), NULL);
		g_signal_connect(buffer, "insert-text", G_CALLBACK(mayus), NULL);
		g_signal_connect(buffer, "delete-range", G_CALLBACK(buffer_borrar), text_view);
		g_signal_connect(text_view, "paste-clipboard", G_CALLBACK(prevent_pegado), NULL);
		g_signal_connect(buffer, "insert-text", G_CALLBACK(corrector), NULL);
        g_signal_connect(menu_item4, "clicked", G_CALLBACK(borrar), NULL);

        
        text_views[i]= text_view;
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
        gtk_box_append (GTK_BOX (text_box), text_view);
    
        gtk_widget_set_size_request(text_view, 700, 60);		//Tamano OG: 700 y 500
        
    	provider = gtk_css_provider_new ();
        gtk_widget_add_css_class(text_view, "text_view");
        gtk_widget_add_css_class(box, "box");


        gtk_css_provider_load_from_data (provider,
                                         "* { font: 15px Monospace; }"
                                         //".box { background-image: linear-gradient(to right, #AD6784, #D9AFBB); }"
                                         ".box { background: url('file:///C:/Users/aoshi/Desktop/Stuff/flowers.png');background - size : cover;background - position : center}"
                                         ".text_view { border: 10px double black; background-color: white; }", -1);

        gtk_style_context_add_provider (gtk_widget_get_style_context (text_view),
                                        GTK_STYLE_PROVIDER (provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        gtk_style_context_add_provider (gtk_widget_get_style_context (box),
                                        GTK_STYLE_PROVIDER (provider),
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                                        
            if (i != 0) {  
            gtk_widget_hide(text_views[i]);
        }
   }
    gtk_widget_show (window);
}


//===========================================FUNCIONES ESENCIALES====================================

/*Esta Funcion hace que nuestro cursor itere sobre los caracteres que escribe el usuario en el cuadro de texto.
Si descomentamos el g_print, nos mostrara en terminal la Linea, y la Columna en la cual nos encontramos.
Si la columna es mayor a 75: Metemos un salto de linea en el buffer
Si la linea es igual a 23 y el contador es menor que 3: Sumamos a nuestro contador, y con esto hacemos que el cuadro de texto aparezca
Tambien hacemos que el cursor se mueva al cuadro de texto siguiente
*/
static void buffer_cambio(GtkTextBuffer *buffer, gpointer data) {
	
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
    gint linea = gtk_text_iter_get_line(&iter) + 1;
    gint columna = gtk_text_iter_get_line_offset(&iter);
    g_print("Posicion del Cursor: Linea %d, Columna %d\n", linea, columna);
    
    if(columna==75){
    	 gtk_text_buffer_insert(buffer, &iter, "\n", -1);
	}
	
	if(linea == 23 && contador<3){
		contador++;
        gtk_widget_set_visible(text_views[contador], TRUE); // Hacemos los text views visibles cuando uno se llena
        gtk_widget_grab_focus(text_views[contador]); 
		       
    }
}

//Esta funcion la llamamos cada vez que borramos algo del buffer
void buffer_borrar(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end, gpointer data) {
    g_idle_add(linea_vacia, data);
}

/*Esta funcion chequea si la linea en la que estamos esta vacia o no. 
Si esta vacia, mueve el cursor hacia la otra linea*/
gboolean linea_vacia(gpointer data) {
    GtkTextView *text_view = GTK_TEXT_VIEW(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view); //Tomamos el buffer del cuadro de texto
    GtkTextIter iter;		//Iterador para saber la posicion del cursor
    
    
		//Tomamos la posicion del cursor
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
    gint columna = gtk_text_iter_get_line_offset(&iter);	//Y aqui tomamos la columna en donde esta el cursor

    // Si la columna esta vacia (El cursor esta en un inicio de linea y termina ahi)
    if (columna == 0 && gtk_text_iter_ends_line(&iter)) {
        mover_cursor(text_view); //Mueve el cursor arriba al final de la otra linea
    }

    return G_SOURCE_REMOVE; // Removemos la funcion "idle" luego de llamarla una vez
}

//Esta funcion es la que nos ayuda  a mover el cursor
void mover_cursor(GtkTextView *text_view) {
    GtkTextIter iter;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);

    // Tomamos Posicion actual del cursor
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

    // Chequeamos si estamos a inicio de linea
    if (gtk_text_iter_starts_line(&iter)) {
        // Si lo estamos, movemos una posicion
        if (gtk_text_iter_backward_line(&iter)) {
            // Y lo movemos hacia el final de linea
            gtk_text_iter_forward_to_line_end(&iter);
        }
    }

}

//Esta Funcion pasa las letras a mayusculas despues de un punto
static void mayus(GtkTextBuffer *buffer,
               GtkTextIter   *location,
               gchar         *text,
               gint           len,
               gpointer       user_data)
{
    if (was_period && g_ascii_islower(text[0])) {
        text[0] = g_ascii_toupper(text[0]);
        was_period = FALSE;
    }

    if (len == 1 && text[0] == ' ') {
        GtkTextIter iter = *location;
        gtk_text_iter_backward_char(&iter);
        was_period = (gtk_text_iter_get_char(&iter) == '.');
    } else if (len == 1 && text[0] == '.') {
        was_period = TRUE;
    } else {
        was_period = FALSE;
    }
}
//Esta funcion nos muestra una ventana de ayuda
void opcion_ayuda(GtkButton *button, gpointer user_data) {
	//Creamos la ventana
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(g_application_get_default()));
    GtkWidget *label = gtk_label_new(" Este programa fue hecho con el fin de guardar notas, apuntes o incluso tareas."
	"\n\n Recuerde, puede escribir hasta la linea 22 y llenar un maximo de 4 paginas.\n"
	"\n Cuando ya haya terminado de escribir y desee guardar su texto,\n puede presionar el boton de Opciones y guardar"
	 " sus apuntes en un archivo .txt\n.Si desea borrar el texto, puede presionar el boton que indica Borrar para hacerlo de manera efectiva ");
    gtk_window_set_title(GTK_WINDOW(window), "Ayuda");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_child(GTK_WINDOW(window), label);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    GtkCssProvider *provider = gtk_css_provider_new();
     gtk_widget_add_css_class(window, "window_ayuda");
        gtk_widget_add_css_class(label, "label_ayuda");
        
    gtk_css_provider_load_from_data(provider,
        ".window_ayuda { background-color: #B0C4DE; }"
        ".label_ayuda { color: #191970; font-family: Arial, sans-serif; font-size: 15px; }", -1);
        
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    
    gtk_widget_show(window);
}

//Esta ventana nos ayuda a cerrar la ventana de la funcion info de manera correcta
void cerrar_ventana(GtkWindow *window, gpointer user_data) {
    gtk_window_destroy(window);
}

//Esta funcion crea una ventana que muestra la informacion del proyecto
void opcion_info(GtkButton *button, gpointer user_data) {
	//Creamos una ventana de dialogo tipo "about"
      GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Bloc de Notas y Editor de texto");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version 1.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Hecho por: Aoshi Blanco. C.I 30.243.721\n"
															"Estudiante de la Universidad Santa Maria\n"
															"\t5to semestre");
	//Ponemos el logo
    GtkWidget *image = gtk_image_new_from_file("C:/Users/aoshi/Desktop/Stuff/N_Smoke.png");
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gtk_image_get_paintable(GTK_IMAGE(image)));

	//conectamos esta senal para que la ventana se cierre de manera correcta
    g_signal_connect(dialog, "close-request", G_CALLBACK(cerrar_ventana), NULL);

    gtk_widget_show(dialog);
}

//Esta funcion nos deja guardar todo lo que escribimos en un archivo txt
void opcion_txt(GtkButton *button, gpointer user_data) {
	
	//Declaramos los punteros al text view actual
  GtkWidget *text_view = (GtkWidget *)user_data;

  // Ponemos un nombre para el archivo
  char notas_guardadas[256];
  
  sprintf(notas_guardadas, "C:/Users/aoshi/Desktop/Project/Notas/Notas.txt");

  // Abrimos el archivo. Usando 'w' para escritura dentro de este.
  FILE *file = fopen(notas_guardadas, "w");

  // loop que pasa sobre los 4 text view
  for (int i = 0; i < 4; i++){
  	
    // Agarra el buffer del text view actual
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_views[i]));
	
    // Estos son los iteradores para el principio y el final del texto en nuestro buffer
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    // Agarra el texto del buffer 
    gchar *text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    // Pasa todo el texto a nuestro archivo
    fputs(text, file);

    // Ponemos esto para que la memoria se libere con cada loop
    g_free(text);
  }

	//Creamos la ventana
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(g_application_get_default()));
    GtkWidget *label = gtk_label_new(" Su texto ha sido guardado exitosamente!\n Su archivo se encuentra guardado en la carpeta 'Notas'. ");
    gtk_window_set_title(GTK_WINDOW(window), "Archivo Guardado correctamente!");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_child(GTK_WINDOW(window), label);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Agregamos CSS para ponerle estilo
    GtkCssProvider *provider = gtk_css_provider_new();
     gtk_widget_add_css_class(window, "window_guardado");
        gtk_widget_add_css_class(label, "label_guardado");
        
    gtk_css_provider_load_from_data(provider,
        ".window_guardado { background-color: #478A59; }"
        ".label_guardado { color: #3E363F; font-family: Arial, sans-serif; font-size: 15px; }", -1);
        
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    
    gtk_widget_show(window);

  // Cierra el archivo
  fclose(file);
  
}

//Esta funcion previene el pegado de texto 
void prevent_pegado(GtkTextView *text_view, gpointer user_data) {
    g_signal_stop_emission_by_name(text_view, "paste-clipboard");
}

//Esta funcion nos ayuda a parar el texto cuando llega a 23 lineas
static void limite_texto(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer data) {
    if (len > 0) {  // Vemos si hay texto
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        gint line_count = gtk_text_iter_get_line(&end) + 1;

        // Vemos si sobrepasa las 22 lineas el texto
        if(line_count > 22 && gtk_text_iter_starts_line(location)){
            g_signal_stop_emission_by_name(buffer, "insert-text");
            // Agarramos el inicio y final de linea
            gtk_text_buffer_get_end_iter(buffer, &end);
            start = end;
            gtk_text_iter_set_line_offset(&start, 0);
            // Borramos la ultima linea
            gtk_text_buffer_delete(buffer, &start, &end);
        }
    }
}

//Esta funcion borra el contenido de los text views
void borrar(GtkWidget *button, gpointer data) {
    for (int i = 0; i < 4; i++) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_views[i]));
        gtk_text_buffer_set_text(buffer, "", 0);
        gtk_widget_set_visible(text_views[1], FALSE); 
    gtk_widget_set_visible(text_views[2], FALSE); 
    gtk_widget_set_visible(text_views[3], FALSE); 

    }
}

static void sugerencia(GtkButton *button, gpointer user_data)
{
  // Obtener la sugerencia del botón
  const gchar *suggestion = gtk_button_get_label(button);

  // Obtener el buffer de texto
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);

  // Obtener la posición del cursor
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

  // Mover el cursor hacia atrás hasta el inicio de la última palabra
  if (!gtk_text_iter_starts_word(&iter))
  {
    gtk_text_iter_backward_word_start(&iter);
  }

  // Guardar la posición de inicio de la última palabra
  GtkTextIter start_iter = iter;

  // Mover el cursor hacia adelante hasta el final de la última palabra
  gtk_text_iter_forward_word_end(&iter);

  // Guardar la posición de final de la última palabra
  GtkTextIter end_iter = iter;

  // Borra la palabra mal escrita
  gtk_text_buffer_delete(buffer, &start_iter, &end_iter);

  gtk_text_buffer_insert(buffer, &start_iter, suggestion, -1);

  // Mueve el cursor al final de la palabra insertada
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
  gtk_text_iter_forward_chars(&iter, g_utf8_strlen(suggestion, -1));
  gtk_text_buffer_place_cursor(buffer, &iter);
}

void corrector(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data)
{

  if (g_ascii_isspace(text[0]) || g_ascii_ispunct(text[0])) 
  {
    // Obtener el inicio y el final de la palabra
    GtkTextIter start = *location; 
    GtkTextIter end = *location;   
    gtk_text_iter_backward_word_start(&start);
    gtk_text_iter_forward_word_end(&end);

    // Leemos la palabra
    gchar *word = gtk_text_iter_get_text(&start, &end);

    // Creamos Hunspell
    Hunhandle *hunspell = Hunspell_create("index.aff", "index.dic"); // Nuestro diccionario

    
    if (Hunspell_spell(hunspell, word) == 0)
    {
      
      char **suggestions;

      int num_suggestions = Hunspell_suggest(hunspell, &suggestions, word);                                 
      GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "underline", PANGO_UNDERLINE_ERROR, NULL); 
      gtk_text_buffer_apply_tag(buffer, tag, &start, &end);                                                 

      if (num_suggestions > 0) 
      {
        GtkWidget *message_window = gtk_window_new();
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        GtkCssProvider *provider;

        gtk_window_set_title(GTK_WINDOW(message_window), "Sugerencias....");
     
        gtk_window_set_default_size(GTK_WINDOW(message_window), 300, 100);
        gtk_window_set_resizable(GTK_WINDOW(message_window), TRUE);

        for (int i = 0; i < num_suggestions; i++)
        {
          // Crear un nuevo botón con la sugerencia como etiqueta
          GtkWidget *suggestion_button = gtk_button_new_with_label(suggestions[i]);

          // Conectar la señal "clicked" al botón de sugerencia
          // Primero, conectas la señal "clicked" del botón a la función 'suggestion_clicked_cb'
          g_signal_connect(suggestion_button, "clicked", G_CALLBACK(sugerencia), buffer);

          // Añadir el botón al box
          gtk_box_append(GTK_BOX(box), suggestion_button);
        }

        gtk_window_set_child(GTK_WINDOW(message_window), box);
        gtk_widget_show(message_window);
      }

      // Liberar las sugerencias
      Hunspell_free_list(hunspell, &suggestions, num_suggestions);
    }
    else
    {
      // La palabra es correcta
      printf("Correcto: %s\n", word);
    }

    // Liberamos memoria
    g_free(word);
    Hunspell_destroy(hunspell);
  }
}

void leyenda(GtkButton *button, gpointer user_data) {
    
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Leyenda");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);  
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    GtkWidget *label = gtk_label_new("Leyenda:\n"
                                 "00 = Espacio\n01 = A\n02 = B\n03 = C\n04 = D\n05 = E\n06 = F\n07 = G\n08 = H\n09 = I\n10 = J\n"
                                 "11 = K\n12 = L\n13 = M\n14 = N\n15 = O\n16 = P\n17 = Q\n18 = R\n19 = S\n20 = T\n21 = U\n22 = V\n"
                                 "23 = W\n24 = X\n25 = Y\n26 = Z");

   
    gtk_window_set_child(GTK_WINDOW(window), label);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_widget_add_css_class(window, "leyendaWindow");

    gtk_css_provider_load_from_data(provider,
        ".leyendaWindow {  background-color: white; font: 13px monospace; }", -1);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show(window);
}

void encriptado(GtkButton *button, gpointer user_data){
    // user_data es lo que el usuario escribe dentro del text view
    GtkTextView *text_encriptado = GTK_TEXT_VIEW(user_data);
     gtk_text_view_set_editable(GTK_TEXT_VIEW(text_encriptado), TRUE);

    // Tomamos el buffer de nuestro cuadro
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_encriptado);
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    // Agarramos el texto del buffer
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    g_print("Texto original: %s\n", text);

    // Array de las letras
    char letters[27] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    // Guardamos el mensaje en un array (Ponemos 200 para asegurarnos que siempre sea suficiente)
    char message[200] = ""; 
    int j = 0;

    for(int i = 0; text[i] != '\0'; i++) {
        // Pasamos las letras a mayuscula
        char c = toupper(text[i]);
        // Usamos strchr para buscar la posicion de la letra en el array
        char *p = strchr(letters, c);
        if(p) {
            int index = p - letters;
            // ya teniendo la posicion, sustituimos
            j += sprintf(&message[j], "%02d", index);
        }
    }
    
    g_print("Mensaje encriptado: %s\n", message);

    //Sustituimos el mensaje original
    gtk_text_buffer_set_text(buffer, message, -1);
    
    // Guardamos el encriptado en un archivo unico
	FILE *file = fopen("C:/Users/aoshi/Desktop/Project/Encriptado/Encriptado.txt", "w");
	if (file != NULL) {
    fputs(message, file);
    fclose(file);
	} else {
    g_print("Error!\n");
	}
    
    g_free(text);
}

void desencriptado(GtkButton *button, gpointer user_data){

    GtkTextView *text_encriptado = GTK_TEXT_VIEW(user_data);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_encriptado), TRUE);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_encriptado);
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    g_print("Texto original: %s\n", text);

    char letters[27] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char message[200] = ""; 
    int j = 0;

    for(int i = 0; text[i] != '\0'; i++) {
    // Convertimos cada par de digitos a enteros
    int num = 0;
   	 if(isdigit(text[i])) {
        num = text[i] - '0';
        	if(isdigit(text[i+1])) {
           	 i++;
            	num = num * 10 + text[i] - '0';
        	}
    	}
    // Juntamos el mensaje encriptado 
    message[j++] = letters[num];
	}
	// Agregamos el caracter nulo al final del mensaje
	message[j] = '\0';

    
    g_print("Mensaje desencriptado: %s\n", message);
    
    gtk_text_buffer_set_text(buffer, message, -1);
    
    FILE *file = fopen("C:/Users/aoshi/Desktop/Project/Encriptado/Encriptado.txt", "w");
    if (file != NULL) {
        fputs(message, file);
        fclose(file);
    } else {
        g_print("Error!\n");
    }
    
    g_free(text);
}

void opcion_encript(GtkButton *button, gpointer user_data) {

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Encriptacion ");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	
    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    GtkWidget *text_encriptado = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_encriptado), GTK_WRAP_CHAR); 

    GtkWidget *button1 = gtk_button_new_with_label("Leyenda");
    GtkWidget *button2 = gtk_button_new_with_label("Encriptado");
    GtkWidget *button3 = gtk_button_new_with_label("Desencriptado");

	g_signal_connect(button1, "clicked", G_CALLBACK(leyenda), NULL);
   	g_signal_connect(button2, "clicked", G_CALLBACK(encriptado), text_encriptado);
    g_signal_connect(button3, "clicked", G_CALLBACK(desencriptado), text_encriptado);

    gtk_widget_add_css_class(button1, "button1");
    gtk_widget_add_css_class(button2, "button2");
    gtk_widget_add_css_class(button3, "button3");

    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button1);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button2);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button3);

    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC); 
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_encriptado);

    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);

    gtk_widget_set_margin_start(frame, 50);
    gtk_widget_set_margin_end(frame, 50);
    gtk_widget_set_margin_top(frame, 50);
    gtk_widget_set_margin_bottom(frame, 50);
    gtk_window_set_child(GTK_WINDOW(window), frame);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_widget_add_css_class(window, "myWindow");
    gtk_widget_add_css_class(text_encriptado, "myTextView");
    
    gtk_css_provider_load_from_data(provider,
    	".myWindow {  background: url('file:///C:/Users/aoshi/Desktop/Stuff/ViceCity.jpeg');background - size : cover;background - position : center }"
        ".myTextView { border: 5px solid #000000; font: 15px Monospace;}"
        ".button1 { background-color: #FF0000; color: #000000; border-radius: 10px; }"  
        ".button2 { background-color: #00FF00; color: #000000; border-radius: 10px; }"  
        ".button3 { background-color: #0000FF; color: #000000; border-radius: 10px; }", 
        -1);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_widget_show(window);
}

//===========================================FUNCION MAIN=========================================================

int main (int argc, char **argv) {
    // Declaramos la app, y la variable del status
    GtkApplication *app;
    int status;
	SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
  	 setlocale(LC_ALL, "es_ES.UTF-8");
    // Creamos la app
    app = gtk_application_new ("com.example.mytexteditor", G_APPLICATION_FLAGS_NONE);
    
    // Conectamos la senal activate  la funcion main (activate= funcion donde esta toda la app)
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    
    // Corremos la app
    status = g_application_run (G_APPLICATION (app), argc, argv);
    
    
    // Ponemos esto para que la app finalize y la memoria se libre y vacie
    g_object_unref (app);

    // Devolvemos el status
    return status;
}

README.txt
==========

Allows users to create & display content in an accordion.
------------------------
Requires - Drupal 7


Overview:
--------
Adds a new button to Drupal 7's CKEditor module which allows the user
to create & display any type of content in an accordion format.

The styling is minimal blue and easily over writeable by developers.


INSTALLATION:
--------
1. Install & Enable the module
2. Open Administration > Configuration > Content authoring >
   Text formats and editors (admin/config/content/formats)
3. Edit a text format's settings (usually Basic HTML)
4. Scroll down to the bottom to "Limit allowed HTML tags" (will only appear if the "Limit allowed HTML tags" filter is enabled)
5. Find and replace <dl> with <dl class>
   This ensures CKEditor doesn't remove the class name that the accordion uses.
6. Open Administration > Configuration > Content authoring >
   CKEditor (admin/config/content/ckeditor)
7. Edit the relevant profile's settings
8. Scroll to "Editor appearance"
9. Under "Plugins" -> Enable "CKEditor Accordion - A plugin to easily create accordions"
10. Under "Toolbar" -> Drag n Drop the Add Accordion -button to the toolbar to show it to the users